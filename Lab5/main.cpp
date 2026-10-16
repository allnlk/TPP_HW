#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// ---------- Константи ----------
const float PI = 3.14159265f;
const float GRAVITY = 980.f;                       // прискорення вільного падіння
const float INCLINE_LENGTH = 600.f;                // довжина площини
const float BODY_WIDTH = 50.f;                     // ширина тіла
const float BODY_HEIGHT = 30.f;                    // висота тіла
const float START_S = 50.f;                        // початкове положення на площині
const sf::Vector2f INCLINE_START{100.f, 150.f};    // верхній край площини
const std::size_t MAX_TRAJECTORY_POINTS = 500;     // обмеження кількості точок

// ---------- Фізична модель ----------
struct Model
{
    float angleDeg = 30.f;   // кут нахилу
    float v0 = 0.f;          // початкова швидкість
    float s = START_S;       // відстань від верхнього краю вздовж площини
    float velocity = 0.f;    // поточна швидкість вздовж площини
    float time = 0.f;        // час руху
    bool running = false;    // чи йде моделювання
    bool finished = false;   // чи дійшло тіло до кінця площини
    std::vector<sf::Vector2f> trajectory; // точки траєкторії (центр тіла)
};

float acceleration(const Model& m)
{
    return GRAVITY * std::sin(m.angleDeg * PI / 180.f);  // a = g * sin(alpha)
}

// Точка на поверхні площини, де стоїть тіло
sf::Vector2f bodyPosition(const Model& m)
{
    float angleRad = m.angleDeg * PI / 180.f;
    return {
        INCLINE_START.x + m.s * std::cos(angleRad),
        INCLINE_START.y + m.s * std::sin(angleRad)
    };
}

// Центр тіла: зсув від поверхні на пів висоти перпендикулярно до площини
sf::Vector2f bodyCenter(const Model& m)
{
    float angleRad = m.angleDeg * PI / 180.f;
    sf::Vector2f p = bodyPosition(m);
    return {
        p.x + (BODY_HEIGHT / 2.f) * std::sin(angleRad),
        p.y - (BODY_HEIGHT / 2.f) * std::cos(angleRad)
    };
}

void resetModel(Model& m)
{
    m.s = START_S;
    m.velocity = m.v0;
    m.time = 0.f;
    m.running = false;
    m.finished = false;
    m.trajectory.clear();
}

// ---------- Оброблення введення ----------
void handleInput(sf::RenderWindow& window, Model& m)
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }

        if (const auto* key = event->getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::Up)    m.angleDeg += 5.f;
            if (key->code == sf::Keyboard::Key::Down)  m.angleDeg -= 5.f;
            if (key->code == sf::Keyboard::Key::Right) m.v0 += 25.f;
            if (key->code == sf::Keyboard::Key::Left)  m.v0 -= 25.f;

            if (key->code == sf::Keyboard::Key::Space && !m.finished)
            {
                m.running = !m.running;
            }
            if (key->code == sf::Keyboard::Key::R)
            {
                resetModel(m);
            }
        }
    }

    // Обмеження параметрів
    m.angleDeg = std::clamp(m.angleDeg, 5.f, 40.f);
    m.v0 = std::clamp(m.v0, 0.f, 300.f);

    // До старту початкова швидкість одразу відображається
    if (m.time == 0.f && !m.running && !m.finished)
    {
        m.velocity = m.v0;
    }
}

// ---------- Оновлення фізичної моделі ----------
void updatePhysics(Model& m, float dt)
{
    if (!m.running || m.finished)
    {
        return;
    }

    float a = acceleration(m);
    m.velocity += a * dt;
    m.s += m.velocity * dt;
    m.time += dt;

    // Кінець площини
    if (m.s + BODY_WIDTH / 2.f >= INCLINE_LENGTH)
    {
        m.s = INCLINE_LENGTH - BODY_WIDTH / 2.f;
        m.velocity = 0.f;
        m.finished = true;
        m.running = false;

        float L = m.s - START_S;
        float tTheory = (-m.v0 + std::sqrt(m.v0 * m.v0 + 2.f * a * L)) / a;
        std::cout << "Angle: " << m.angleDeg << " deg, v0: " << m.v0 << "\n";
        std::cout << "Час (чисельно): " << m.time << " c\n";
        std::cout << "Час (теорія):   " << tTheory << " c\n\n";
    }

    // Траєкторія
    m.trajectory.push_back(bodyCenter(m));
    if (m.trajectory.size() > MAX_TRAJECTORY_POINTS)
    {
        m.trajectory.erase(m.trajectory.begin());
    }
}

// ---------- Відображення ----------
struct Scene
{
    sf::RectangleShape ground;
    sf::RectangleShape incline;
    sf::RectangleShape body;
    sf::VertexArray trajectoryLine;
    sf::Text info;

    explicit Scene(const sf::Font& font)
        : ground({800.f, 50.f}),
          incline({INCLINE_LENGTH, 20.f}),
          body({BODY_WIDTH, BODY_HEIGHT}),
          trajectoryLine(sf::PrimitiveType::LineStrip),
          info(font, "", 20)
    {
        ground.setFillColor(sf::Color(100, 100, 100));
        ground.setPosition({0.f, 550.f});

        incline.setFillColor(sf::Color(160, 160, 160));
        incline.setPosition(INCLINE_START);

        body.setFillColor(sf::Color::White);
        body.setOrigin({BODY_WIDTH / 2.f, BODY_HEIGHT});  // середина нижньої сторони

        info.setFillColor(sf::Color::White);
        info.setPosition({20.f, 20.f});
    }
};

void drawScene(sf::RenderWindow& window, Scene& sc, const Model& m)
{
    sc.incline.setRotation(sf::degrees(m.angleDeg));
    sc.body.setRotation(sf::degrees(m.angleDeg));
    sc.body.setPosition(bodyPosition(m));

    // Лінія траєкторії
    sc.trajectoryLine.clear();
    for (const auto& point : m.trajectory)
    {
        sc.trajectoryLine.append(sf::Vertex{point, sf::Color::Yellow});
    }

    std::string status = m.finished ? "Finished" : (m.running ? "Running" : "Paused");

    sc.info.setString(
        "Angle: " + std::to_string(m.angleDeg) +
        "\nAcceleration: " + std::to_string(acceleration(m)) +
        "\nInitial velocity: " + std::to_string(m.v0) +
        "\nVelocity: " + std::to_string(m.velocity) +
        "\nPosition: " + std::to_string(m.s) +
        "\nDistance: " + std::to_string(m.s - START_S) +
        "\nTime: " + std::to_string(m.time) +
        "\nStatus: " + status +
        "\n\nSpace: start/pause   R: restart" +
        "\nUp/Down: angle   Left/Right: v0"
    );

    window.clear(sf::Color::Black);
    window.draw(sc.ground);
    window.draw(sc.incline);
    window.draw(sc.trajectoryLine);   // траєкторія поверх площини, під тілом
    window.draw(sc.body);
    window.draw(sc.info);
    window.display();
}

// ---------- Головна функція ----------
int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "SFML Physics Demo"
    );

    sf::Font font;
    if (!font.openFromFile("../resources/fonts/arial.ttf"))
    {
        return 1;
    }

    Scene scene(font);
    Model model;
    sf::Clock clock;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();

        handleInput(window, model);      // 1. Оброблення подій
        updatePhysics(model, dt);        // 2. Оновлення фізичної моделі
        drawScene(window, scene, model); // 3. Відображення сцени
    }
    return 0;
}

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <optional>

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "SFML Physics Demo"
    );

    const float PI = 3.14159265f;
    const float angleDeg = 30.f;                   // кут нахилу
    const float angleRad = angleDeg * PI / 180.f;
    const float inclineLength = 600.f;             // довжина площини
    const sf::Vector2f inclineStart{100.f, 150.f}; // верхній край площини
    const float bodyWidth = 50.f;

    // Підлога
    sf::RectangleShape ground({800.f, 50.f});
    ground.setFillColor(sf::Color(100, 100, 100));
    ground.setPosition({0.f, 550.f});

    // Похила площина
    sf::RectangleShape incline({inclineLength, 20.f});
    incline.setFillColor(sf::Color(160, 160, 160));
    incline.setPosition(inclineStart);
    incline.setRotation(sf::degrees(angleDeg));

    // Тіло на площині
    const float startS = 50.f;
    float s = startS; // відстань від верхнього краю вздовж площини
    sf::RectangleShape body({bodyWidth, 30.f});
    body.setFillColor(sf::Color::White);
    body.setOrigin({bodyWidth / 2.f, 30.f}); // середина нижньої сторони
    body.setRotation(sf::degrees(angleDeg));

    // Фізична модель
    float velocity = 0.f;          // швидкість вздовж площини
    const float gravity = 980.f;   // прискорення вільного падіння
    float time = 0.f;              // час руху
    bool finished = false;         // чи дійшло тіло до кінця площини
    sf::Clock clock;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        // Оновлення фізичної моделі
        float dt = clock.restart().asSeconds();

        if (!finished)
        {
            float a = gravity * std::sin(angleRad);  // a = g * sin(alpha)
            velocity += a * dt;
            s += velocity * dt;
            time += dt;

            // Кінець площини
            if (s + bodyWidth / 2.f >= inclineLength)
            {
                s = inclineLength - bodyWidth / 2.f;
                velocity = 0.f;
                finished = true;

                float tTheory = std::sqrt(2.f * (s - startS) / a);
                std::cout << "Час (чисельно): " << time << " c\n";
                std::cout << "Час (теорія):   " << tTheory << " c\n";
            }
        }

        body.setPosition({
            inclineStart.x + s * std::cos(angleRad),
            inclineStart.y + s * std::sin(angleRad)
        });

        // Відображення
        window.clear(sf::Color::Black);
        window.draw(ground);
        window.draw(incline);
        window.draw(body);
        window.display();
    }
    return 0;
}

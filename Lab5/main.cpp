#include <SFML/Graphics.hpp>
#include <cmath>
#include <optional>

int main()
{
    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "SFML Physics Demo"
    );

    const float PI = 3.14159265f;
    const float angleDeg = 30.f;                  // кут нахилу
    const float angleRad = angleDeg * PI / 180.f;
    const float inclineLength = 600.f;            // довжина площини
    const sf::Vector2f inclineStart{100.f, 150.f}; // верхній край площини

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
    float s = 50.f; // відстань від верхнього краю вздовж площини
    sf::RectangleShape body({50.f, 30.f});
    body.setFillColor(sf::Color::White);
    body.setOrigin({25.f, 30.f}); // середина нижньої сторони
    body.setRotation(sf::degrees(angleDeg));
    body.setPosition({
        inclineStart.x + s * std::cos(angleRad),
        inclineStart.y + s * std::sin(angleRad)
    });

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        window.draw(ground);
        window.draw(incline);
        window.draw(body);
        window.display();
    }
    return 0;
}

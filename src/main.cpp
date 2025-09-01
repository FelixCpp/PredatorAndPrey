#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "SFML works!");
    window.setFramerateLimit(60);

    while (window.isOpen())
    {
        window.handleEvents([&window](const sf::Event::Closed&) { window.close(); });
        window.clear(sf::Color::Black);
        window.display();
    }

    return 0;
}

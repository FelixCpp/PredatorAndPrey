#include <SFML/Graphics.hpp>

#include <random>
#include <vector>

enum class Creature
{
    Empty,
    Predator,
    Prey,
};

namespace
{
inline constexpr int maxHealth = 100;
std::mt19937         generator{std::random_device{}()};
Creature             getRandomCreature();
sf::Color            getCreatureColor(Creature creature, int health);
sf::Vector2i         getNeighborCoordinates(int x, int y);
} // namespace

int main()
{
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(900, 900)), "SFML works!");
    window.setFramerateLimit(60);

    const auto [windowWidth, windowHeight] = window.getSize();
    const int columns                      = 400;
    const int rows                         = 400;

    sf::Image          image(sf::Vector2u(columns, rows), sf::Color::Transparent);
    sf::Texture        texture(sf::Vector2u(columns, rows));
    sf::RectangleShape shape(sf::Vector2f(static_cast<float>(windowWidth), static_cast<float>(windowHeight)));
    shape.setTexture(&texture);

    std::vector<Creature> creatures(columns * rows, Creature::Empty);
    std::vector<int>      healths(columns * rows, maxHealth);

    // Initialize the grid with random creatures.
    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < columns; ++x)
        {
            creatures[y * columns + x] = getRandomCreature();
        }
    }

    while (window.isOpen())
    {
        window.handleEvents([&window](const sf::Event::Closed&) { window.close(); },
                            [&window](const sf::Event::KeyPressed& event)
                            {
                                if (event.code == sf::Keyboard::Key::Escape)
                                {
                                    window.close();
                                }
                            });

        for (int y = rows - 1; y >= 0; --y)
        // for (int y = 0; y < rows; ++y)
        {
            for (int x = columns - 1; x >= 0; --x)
            // for (int x = 0; x < columns; ++x)
            {
                const int index    = y * columns + x;
                Creature& creature = creatures[index];
                int&      health   = healths[index];

                switch (creature)
                {
                    case Creature::Empty:
                        break;
                    case Creature::Predator:
                        health = std::max(health - 1, 0);
                        break;
                    case Creature::Prey:
                        health = std::min(health + 5, maxHealth);
                        break;
                }

                const auto [neighborX, neighborY] = getNeighborCoordinates(x, y);
                const bool isOutOfSight   = neighborX < 0 or neighborX >= columns or neighborY < 0 or neighborY >= rows;
                const bool isSameCreature = neighborX == x and neighborY == y;
                if (not isOutOfSight and not isSameCreature)
                {
                    const Creature neighbor = creatures[neighborY * columns + neighborX];

                    const auto move = [&creatures, &healths](int cellX, int cellY, int targetX, int targetY)
                    {
                        const int cellIndex   = cellY * columns + cellX;
                        const int targetIndex = targetY * columns + targetX;

                        creatures[targetIndex] = creatures[cellIndex];
                        healths[targetIndex]   = healths[cellIndex];
                        creatures[cellIndex]   = Creature::Empty;
                        healths[cellIndex]     = 0;
                    };

                    const auto heal = [&healths](const int cellX, const int cellY, const int amount)
                    {
                        const int index = cellY * columns + cellX;
                        healths[index]  = std::clamp(healths[index] + amount, 0, maxHealth);
                    };


                    switch (creature)
                    {
                        case Creature::Empty:
                            break;
                        case Creature::Predator:
                        {
                            if (healths[index] <= 0)
                            {
                                creatures[index] = Creature::Empty;
                                break;
                            }

                            switch (neighbor)
                            {
                                case Creature::Empty:
                                {
                                    move(x, y, neighborX, neighborY);
                                }
                                break;
                                case Creature::Predator:
                                    break;

                                case Creature::Prey:
                                {
                                    creatures[neighborY * columns + neighborX] = Creature::Predator;
                                    healths[index] = std::min(healths[index] + healths[neighborY * columns + neighborX] / 4,
                                                              maxHealth);
                                }
                                break;
                            }
                        }
                        break;
                        case Creature::Prey:
                        {
                            const bool shouldReproduce = true; // healths[index] >= maxHealth;
                            if (shouldReproduce)
                            {
                                healths[index] = 10;
                            }

                            switch (neighbor)
                            {
                                case Creature::Empty:
                                {
                                    if (shouldReproduce)
                                    {
                                        creatures[neighborY * columns + neighborX] = Creature::Prey;
                                        healths[neighborY * columns + neighborX]   = 10;
                                    }
                                    else
                                    {
                                        move(x, y, neighborX, neighborY);
                                    }
                                }
                                break;
                                case Creature::Predator:
                                    break;

                                case Creature::Prey:
                                    break;
                            }
                        }
                        break;
                    }
                }

                image.setPixel(sf::Vector2u(x, y), getCreatureColor(creature, health));
            }
        }

        texture.update(image);

        window.clear(sf::Color::Black);
        window.draw(shape);
        window.display();
    }

    return 0;
}

namespace
{
Creature getRandomCreature()
{
    static std::uniform_int_distribution<int> distribution(0, 100);
    const int                                 value = distribution(generator);

    if (value > 50)
    {
        return Creature::Empty;
    }

    if (value > 5)
    {
        return Creature::Prey;
    }

    return Creature::Predator;
}
sf::Color getCreatureColor(Creature creature, int health)
{
    const float healthRatio = static_cast<float>(health) / static_cast<float>(maxHealth);
    const auto  alpha       = static_cast<uint8_t>(std::clamp(healthRatio * 255.0f, 0.0f, 255.0f));

    switch (creature)
    {
        case Creature::Empty:
            return sf::Color::Transparent;
        case Creature::Predator:
            return {255, 0, 0, alpha};
        case Creature::Prey:
            return {0, 255, 0, alpha};
    }
}
sf::Vector2i getNeighborCoordinates(int x, int y)
{
    static std::uniform_int_distribution<int> distribution(-1, 1);
    const int                                 newX = x + distribution(generator);
    const int                                 newY = y + distribution(generator);
    return {newX, newY};
}
} // namespace

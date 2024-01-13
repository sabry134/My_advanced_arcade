#include <SFML/Graphics.hpp>
#include <iostream>
#include <deque>
#include <chrono>
#include <thread>
#include "directions.hpp"

class RTypeGame
{
public:
    RTypeGame(sf::RenderWindow &window) : window(window), health(3)
    {
        initialize();
        clock.restart();
        shooting = false;
        shootingClock.restart();
    }

    void run()
    {
        while (window.isOpen() && health > 0)
        {
            handleEvents();
            update();
            render();
            wait();
        }

        std::cout << "Game Over!" << std::endl;
    }

private:
    sf::RenderWindow &window;
    sf::RectangleShape playerShip;
    std::deque<sf::RectangleShape> enemyShips;
    std::deque<sf::RectangleShape> bullets;
    Direction playerDirection;
    bool shooting;
    sf::Clock clock;
    sf::Clock shootingClock;
    int health;

    float shootingInterval = 0.5f;

    sf::Font font;
    sf::Text healthText;

    void initialize()
    {
        playerShip.setSize(sf::Vector2f(50, 50));
        playerShip.setPosition(window.getSize().x / 2 - 25, window.getSize().y - 60);
        playerShip.setFillColor(sf::Color::Blue);

        playerDirection = NONE;

        spawnEnemyShips(5);

        if (!font.loadFromFile("arial.ttf"))
        {
            std::cerr << "Error loading font file!" << std::endl;
        }

        healthText.setFont(font);
        healthText.setCharacterSize(20);
        healthText.setFillColor(sf::Color::White);
        healthText.setPosition(10, 10);
        updateHealthText();
    }

    void handleEvents()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                handleKeyPress(event.key.code);
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                handleKeyRelease(event.key.code);
            }
        }
    }

    void handleKeyPress(sf::Keyboard::Key key)
    {
        switch (key)
        {
        case sf::Keyboard::Left:
            playerDirection = LEFT;
            break;
        case sf::Keyboard::Right:
            playerDirection = RIGHT;
            break;
        case sf::Keyboard::Up:
            playerDirection = UP;
            break;
        case sf::Keyboard::Down:
            playerDirection = DOWN;
            break;
        case sf::Keyboard::Space:
            shooting = true;
            break;
        default:
            break;
        }
    }

    void handleKeyRelease(sf::Keyboard::Key key)
    {
        if (key == sf::Keyboard::Space)
        {
            shooting = false;
        }
    }

    void update()
    {
        movePlayerShip();
        moveEnemyShips();

        handleShooting();

        checkCollisions();

        moveBullets();
    }

    void handleShooting()
    {
        if (shooting && shootingClock.getElapsedTime().asSeconds() >= shootingInterval)
        {
            shoot();
            shootingClock.restart();
        }
    }

    void checkCollisions()
    {
        for (auto enemyIter = enemyShips.begin(); enemyIter != enemyShips.end();)
        {
            if (playerShip.getGlobalBounds().intersects(enemyIter->getGlobalBounds()))
            {
                health -= 1;
                std::cout << "Health: " << health << std::endl;

                enemyIter = enemyShips.erase(enemyIter);

                if (health <= 0)
                {
                    window.close();
                    return;
                }
            }
            else
            {
                ++enemyIter;
            }
        }

        for (auto bulletIter = bullets.begin(); bulletIter != bullets.end();)
        {
            bool bulletRemoved = false;

            for (auto enemyIter = enemyShips.begin(); enemyIter != enemyShips.end();)
            {
                if (bulletIter->getGlobalBounds().intersects(enemyIter->getGlobalBounds()))
                {
                    bulletIter = bullets.erase(bulletIter);
                    enemyIter = enemyShips.erase(enemyIter);
                    bulletRemoved = true;
                    break;
                }
                else
                {
                    ++enemyIter;
                }
            }

            if (!bulletRemoved)
            {
                ++bulletIter;
            }
        }

        if (enemyShips.empty())
        {
            spawnEnemyShips(5);
        }

        updateHealthText();
    }

    void updateHealthText()
    {
        healthText.setString("Health: " + std::to_string(health));
    }

    void render()
    {
        window.clear();

        window.draw(healthText);

        window.draw(playerShip);

        for (const auto &enemyShip : enemyShips)
        {
            window.draw(enemyShip);
        }

        for (const auto &bullet : bullets)
        {
            window.draw(bullet);
        }

        window.display();
    }

    void movePlayerShip()
    {
        const float playerSpeed = 5.0f;

        switch (playerDirection)
        {
        case LEFT:
            playerShip.move(-playerSpeed, 0);
            break;
        case RIGHT:
            playerShip.move(playerSpeed, 0);
            break;
        case UP:
            if (playerShip.getPosition().y > 0)
                playerShip.move(0, -playerSpeed);
            break;
        case DOWN:
            if (playerShip.getPosition().y + playerShip.getSize().y < window.getSize().y)
                playerShip.move(0, playerSpeed);
            break;
        default:
            break;
        }

        if (playerShip.getPosition().x < 0)
            playerShip.setPosition(window.getSize().x, playerShip.getPosition().y);
        else if (playerShip.getPosition().x > window.getSize().x)
            playerShip.setPosition(0, playerShip.getPosition().y);
    }

    void moveEnemyShips()
    {
        const float enemySpeed = 3.0f;

        for (auto &enemyShip : enemyShips)
        {
            enemyShip.move(0, enemySpeed);

            if (enemyShip.getPosition().y > window.getSize().y)
            {
                enemyShip.setPosition(rand() % (window.getSize().x - 50), 0);
            }
        }
    }

    void shoot()
    {
        sf::RectangleShape bullet(sf::Vector2f(5, 10));
        bullet.setPosition(playerShip.getPosition().x + playerShip.getSize().x / 2 - 2.5,
                           playerShip.getPosition().y);
        bullet.setFillColor(sf::Color::Green);
        bullets.push_back(bullet);
    }

    void moveBullets()
    {
        const float bulletSpeed = 8.0f;

        for (auto &bullet : bullets)
        {
            bullet.move(0, -bulletSpeed);

            if (bullet.getPosition().y < 0)
            {
                bullets.pop_front();
            }
        }
    }

    void spawnEnemyShips(int count)
    {
        for (int i = 0; i < count; ++i)
        {
            sf::RectangleShape enemyShip(sf::Vector2f(50, 50));
            enemyShip.setPosition(rand() % (window.getSize().x - 50), -50);
            enemyShip.setFillColor(sf::Color::Red);

            enemyShips.push_back(enemyShip);
        }
    }

    void wait()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
};

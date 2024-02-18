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
    sf::Sprite background;
    sf::Sprite playerShip;
    std::deque<sf::Sprite> enemyShips;
    std::deque<sf::RectangleShape> bullets;
    std::deque<sf::RectangleShape> playerBullets;
    std::deque<sf::RectangleShape> enemyBullets;
    Direction playerDirection;
    bool shooting;
    sf::Clock clock;
    sf::Clock shootingClock;
    int health;

    float shootingInterval = 0.5f;

    sf::Texture playerTexture;
    sf::Texture enemyTexture;
    sf::Texture backgroundTexture;

    sf::Font font;
    sf::Text healthText;

    void initialize()
    {
        if (!playerTexture.loadFromFile("spaceship.png"))
        {
            std::cerr << "Error loading player texture!" << std::endl;
        }

        if (!enemyTexture.loadFromFile("enemy.png"))
        {
            std::cerr << "Error loading enemy texture!" << std::endl;
        }

        if (!backgroundTexture.loadFromFile("space.jpg"))
        {
            std::cerr << "Error loading background texture!" << std::endl;
        }

        background.setTexture(backgroundTexture);

        playerShip.setTexture(playerTexture);
        playerShip.setPosition(window.getSize().x / 2 - playerShip.getGlobalBounds().width / 2, window.getSize().y - 60);

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
        handleEnemyShooting();

        checkCollisions();

        movePlayerBullets();
        moveEnemyBullets();
    }

    void handleEnemyShooting()
    {
        for (auto &enemyShip : enemyShips)
        {
            if (rand() % 100 < 5)
            {
                enemyShoot(enemyShip.getPosition().x + enemyShip.getGlobalBounds().width / 2 - 2.5,
                           enemyShip.getPosition().y + enemyShip.getGlobalBounds().height);
            }
        }
    }

    void enemyShoot(float x, float y)
    {
        sf::RectangleShape enemyBullet(sf::Vector2f(5, 10));
        enemyBullet.setPosition(x - 2.5, y);
        enemyBullet.setFillColor(sf::Color::Red);
        enemyBullets.push_back(enemyBullet);
    }

    void moveEnemyBullets()
    {
        const float enemyBulletSpeed = 5.0f;

        for (auto &bullet : enemyBullets)
        {
            bullet.move(0, enemyBulletSpeed);

            if (bullet.getPosition().y > window.getSize().y)
            {
                enemyBullets.pop_front();
            }
        }
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

        for (auto bulletIter = playerBullets.begin(); bulletIter != playerBullets.end();)
        {
            for (auto enemyIter = enemyShips.begin(); enemyIter != enemyShips.end();)
            {
                if (bulletIter->getGlobalBounds().intersects(enemyIter->getGlobalBounds()))
                {
                    bulletIter = playerBullets.erase(bulletIter);
                    enemyIter = enemyShips.erase(enemyIter);
                    break;
                }
                else
                {
                    ++enemyIter;
                }
            }

            if (bulletIter != playerBullets.end())
            {
                ++bulletIter;
            }
        }

        for (auto bulletIter = enemyBullets.begin(); bulletIter != enemyBullets.end();)
        {
            if (playerShip.getGlobalBounds().intersects(bulletIter->getGlobalBounds()))
            {
                health -= 1;
                std::cout << "Health: " << health << std::endl;

                bulletIter = enemyBullets.erase(bulletIter);

                if (health <= 0)
                {
                    window.close();
                    return;
                }
            }
            else
            {
                ++bulletIter;
            }
        }

        if (enemyShips.empty())
        {
            spawnEnemyShips(10);
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

        window.draw(background);

        window.draw(healthText);

        window.draw(playerShip);

        for (const auto &enemyShip : enemyShips)
        {
            window.draw(enemyShip);
        }

        for (const auto &bullet : playerBullets)
        {
            window.draw(bullet);
        }

        for (const auto &bullet : enemyBullets)
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
            if (playerShip.getPosition().y + playerShip.getGlobalBounds().height < window.getSize().y)
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
        sf::RectangleShape playerBullet(sf::Vector2f(5, 10));
        playerBullet.setPosition(playerShip.getPosition().x + playerShip.getGlobalBounds().width / 2 - 2.5,
                                 playerShip.getPosition().y);
        playerBullet.setFillColor(sf::Color::Green);
        playerBullets.push_back(playerBullet);
    }

    void movePlayerBullets()
    {
        const float playerBulletSpeed = 8.0f;

        for (auto &bullet : playerBullets)
        {
            bullet.move(0, -playerBulletSpeed);

            if (bullet.getPosition().y < 0)
            {
                playerBullets.pop_front();
            }
        }
    }

    void spawnEnemyShips(int count)
    {
        for (int i = 0; i < count; ++i)
        {
            sf::Sprite enemyShip(enemyTexture);
            enemyShip.setPosition(rand() % (window.getSize().x - 50), -50);

            enemyShips.push_back(enemyShip);
        }
    }

    void wait()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
};

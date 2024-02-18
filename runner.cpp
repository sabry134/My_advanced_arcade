#include <SFML/Graphics.hpp>
#include <iostream>
#include <deque>
#include <chrono>
#include <thread>

class RunnerGame
{
public:
    RunnerGame(sf::RenderWindow &window) : window(window), playerTexture(), player(), isJumping(false), jumpHeight(0), maxJumpHeight(50.0f), coinCount(0)
    {
        initialize();
        clock.restart();

        if (!font.loadFromFile("arial.ttf"))
        {
            std::cerr << "Failed to load font!" << std::endl;
        }

        coinText.setFont(font);
        coinText.setCharacterSize(20);
        coinText.setFillColor(sf::Color::White);
        coinText.setPosition(10, 10);

        obstacleSpeed = 5.0f;
        obstacleTimer.restart();

        if (!rocketTexture.loadFromFile("rocket.png"))
        {
            std::cerr << "Failed to load rocket texture!" << std::endl;
        }

        if (!alienTexture.loadFromFile("alien.png"))
        {
            std::cerr << "Failed to load alien texture!" << std::endl;
        }

        // Load player texture
        if (!playerTexture.loadFromFile("jetpack.png"))
        {
            std::cerr << "Failed to load player texture!" << std::endl;
        }

        player.setTexture(playerTexture);
        player.setPosition(700, window.getSize().y - player.getGlobalBounds().height - 20);

        // Load background texture
        if (!backgroundTexture.loadFromFile("space_map.jpg"))
        {
            std::cerr << "Failed to load background texture!" << std::endl;
        }

        background.setTexture(backgroundTexture);
    }

    void run()
    {
        while (window.isOpen())
        {
            handleEvents();
            update();
            render();
            wait();
        }
    }

private:
    sf::RenderWindow &window;
    sf::Texture playerTexture;  // Added player texture
    sf::Sprite player;          // Changed player to sprite
    sf::Texture backgroundTexture;  // Added background texture
    sf::Sprite background;          // Added background sprite
    std::deque<sf::RectangleShape> obstacles;
    std::deque<sf::Sprite> rockets;
    std::deque<sf::RectangleShape> coins;
    bool isJumping;
    float jumpHeight;
    float maxJumpHeight;
    sf::Clock clock;
    int coinCount;

    sf::Font font;
    sf::Text coinText;

    float obstacleSpeed;
    sf::Clock obstacleTimer;

    sf::Texture rocketTexture;
    sf::Texture alienTexture;

    void initialize()
    {
        spawnObstacle();
        spawnRocket();
        spawnCoin();
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
                if (event.key.code == sf::Keyboard::Space && !isJumping)
                {
                    isJumping = true;
                    jumpHeight = 0;
                }
            }
        }
    }

    void moveCoins()
    {
        const float coinSpeed = 5.0f;

        for (auto &coin : coins)
        {
            coin.move(-coinSpeed, 0);

            if (coin.getPosition().x + coin.getSize().x < 0)
            {
                coins.pop_front();
            }
        }
    }

    void update()
    {
        movePlayer();
        moveObstacles();
        moveRockets();
        moveCoins();
        checkCollisions();
        spawnObjects();
        updateObstacleSpeed();
    }

    void updateObstacleSpeed()
    {
        if (obstacleTimer.getElapsedTime().asSeconds() >= 5.0)
        {
            obstacleSpeed += 1.0f;
            obstacleTimer.restart();
        }
    }

    void render()
    {
        window.clear();

        // Draw background first
        window.draw(background);

        // Then draw other elements
        window.draw(player);

        for (const auto &obstacle : obstacles)
        {
            window.draw(obstacle);
        }

        for (const auto &rocket : rockets)
        {
            window.draw(rocket);
        }

        for (const auto &coin : coins)
        {
            window.draw(coin);
        }

        coinText.setString("Coins: " + std::to_string(coinCount));
        window.draw(coinText);

        window.display();
    }

    void wait()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    void movePlayer()
    {
        const float jumpSpeed = 5.0f;
        const float gravity = 5.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && player.getPosition().y > 0)
        {
            player.move(0, -jumpSpeed);

            if (player.getPosition().y < 0)
            {
                player.setPosition(player.getPosition().x, 0);
            }
        }
        else
        {
            player.move(0, gravity);

            if (player.getPosition().y > window.getSize().y - player.getGlobalBounds().height - 20)
            {
                player.setPosition(player.getPosition().x, window.getSize().y - player.getGlobalBounds().height - 20);
            }
        }
    }

    void moveObstacles()
    {
        for (auto &obstacle : obstacles)
        {
            obstacle.move(-obstacleSpeed, 0);

            if (obstacle.getPosition().x + obstacle.getSize().x < 0)
            {
                obstacles.pop_front();
            }
        }
    }

    void moveRockets()
    {
        const float rocketSpeed = 8.0f;

        for (auto &rocket : rockets)
        {
            rocket.move(-rocketSpeed, 0);

            if (rocket.getPosition().x + rocket.getGlobalBounds().width < 0)
            {
                rockets.pop_front();
            }
        }
    }

    void checkCollisions()
    {
        for (const auto &obstacle : obstacles)
        {
            if (player.getGlobalBounds().intersects(obstacle.getGlobalBounds()))
            {
                std::cout << "Game Over! Hit obstacle." << std::endl;
                window.close();
            }
        }

        for (const auto &rocket : rockets)
        {
            if (player.getGlobalBounds().intersects(rocket.getGlobalBounds()))
            {
                std::cout << "Game Over! Hit rocket." << std::endl;
                window.close();
            }
        }

        for (auto it = coins.begin(); it != coins.end();)
        {
            if (player.getGlobalBounds().intersects(it->getGlobalBounds()))
            {
                coinCount++;
                std::cout << "Coins Collected: " << coinCount << std::endl;

                it = coins.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void spawnObstacle()
    {
        sf::RectangleShape obstacle;

        obstacle.setSize(sf::Vector2f(50, 50));
        obstacle.setTexture(&alienTexture);

        int yPos = rand() % static_cast<int>(window.getSize().y - obstacle.getSize().y);
        obstacle.setPosition(window.getSize().x, static_cast<float>(yPos));

        obstacles.push_back(obstacle);
    }

    void spawnRocket()
    {
        sf::Sprite rocket(rocketTexture);
        rocket.setPosition(window.getSize().x, rand() % static_cast<int>(window.getSize().y - rocket.getGlobalBounds().height));

        rockets.push_back(rocket);
    }

    void spawnCoin()
    {
        sf::RectangleShape coin(sf::Vector2f(15, 15));
        coin.setFillColor(sf::Color::Yellow);
        int yPos = rand() % static_cast<int>(window.getSize().y - coin.getSize().y);
        coin.setPosition(window.getSize().x, static_cast<float>(yPos));

        coins.push_back(coin);
    }

    void spawnObjects()
    {
        if (rand() % 100 < 1)
        {
            spawnObstacle();
        }

        if (rand() % 100 < 0.5)
        {
            spawnRocket();
        }

        if (rand() % 100 < 1)
        {
            spawnCoin();
        }
    }
};

#include <SFML/Graphics.hpp>
#include <iostream>
#include <deque>
#include <chrono>
#include <thread>
#include "directions.hpp"

class SnakeGame
{
public:
    SnakeGame(sf::RenderWindow &window) : window(window), snakeSize(20), direction(RIGHT)
    {
        initialize();
        clock.restart();
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
    std::deque<sf::RectangleShape> snake;
    sf::RectangleShape food;
    int snakeSize;
    Direction direction;
    sf::Clock clock;

    void initialize()
    {
        snake.clear();
        snake.push_front(createSnakeSegment(100, 100));
        generateFood();
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
        }
    }

    void handleKeyPress(sf::Keyboard::Key key)
    {
        switch (key)
        {
        case sf::Keyboard::Up:
            if (direction != DOWN)
                direction = UP;
            break;
        case sf::Keyboard::Down:
            if (direction != UP)
                direction = DOWN;
            break;
        case sf::Keyboard::Left:
            if (direction != RIGHT)
                direction = LEFT;
            break;
        case sf::Keyboard::Right:
            if (direction != LEFT)
                direction = RIGHT;
            break;
        default:
            break;
        }
    }

    void update()
    {
        if (clock.getElapsedTime().asMilliseconds() > 100)
        {
            moveSnake();
            checkCollision();
            checkFoodCollision();
            clock.restart();
        }
    }

    void render()
    {
        window.clear();
        drawSnake();
        drawFood();
        window.display();
    }

    void moveSnake()
    {
        sf::Vector2f headPosition = snake.front().getPosition();
        switch (direction)
        {
        case UP:
            headPosition.y -= snakeSize;
            break;
        case DOWN:
            headPosition.y += snakeSize;
            break;
        case LEFT:
            headPosition.x -= snakeSize;
            break;
        case RIGHT:
            headPosition.x += snakeSize;
            break;
        default:
            break;
        }

        snake.push_front(createSnakeSegment(headPosition.x, headPosition.y));

        if (snake.size() > 1)
        {
            snake.pop_back();
        }
    }

    void checkCollision()
    {
        sf::Vector2f headPosition = snake.front().getPosition();

        if (headPosition.x < 0 || headPosition.x >= window.getSize().x ||
            headPosition.y < 0 || headPosition.y >= window.getSize().y)
        {
            initialize();
        }

        for (auto it = snake.begin() + 1; it != snake.end(); ++it)
        {
            if (headPosition == it->getPosition())
            {
                initialize();
                break;
            }
        }
    }

    void checkFoodCollision()
    {
        sf::Vector2f headPosition = snake.front().getPosition();
        if (headPosition == food.getPosition())
        {
            snake.push_back(createSnakeSegment(-snakeSize, -snakeSize));
            generateFood();
        }
    }

    void generateFood()
    {
        int maxX = (window.getSize().x / snakeSize) - 1;
        int maxY = (window.getSize().y / snakeSize) - 1;

        int x = rand() % maxX * snakeSize;
        int y = rand() % maxY * snakeSize;

        food.setSize(sf::Vector2f(snakeSize, snakeSize));
        food.setPosition(x, y);
        food.setFillColor(sf::Color::Green);
    }

    sf::RectangleShape createSnakeSegment(float x, float y)
    {
        sf::RectangleShape segment(sf::Vector2f(snakeSize, snakeSize));
        segment.setPosition(x, y);
        segment.setFillColor(sf::Color::White);
        return segment;
    }

    void drawSnake()
    {
        for (const auto &segment : snake)
        {
            window.draw(segment);
        }
    }

    void drawFood()
    {
        window.draw(food);
    }

    void wait()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
};
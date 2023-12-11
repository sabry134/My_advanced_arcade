#include <SFML/Graphics.hpp>
#include <iostream>

enum GameOption {
    SNAKE,
    PACMAN,
    NIBBLER,
    RTYPE,
    RUNNER,
    FIX_IT_FELIX,
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Game Menu");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font." << std::endl;
        return 1;
    }

    sf::Text snakeText("1. Snake", font, 30);
    sf::Text pacmanText("2. Pacman", font, 30);
    sf::Text nibblerText("3. Nibbler", font, 30);
    sf::Text rtypetext("4. R-Type", font, 30);
    sf::Text runnertext("5. Runner", font, 30);
    sf::Text fixItFelixText("6. Fix it Felix Jr.", font, 30);

    snakeText.setPosition(100, 100);
    pacmanText.setPosition(100, 150);
    nibblerText.setPosition(100, 200);
    rtypetext.setPosition(100, 250);
    runnertext.setPosition(100, 300);
    fixItFelixText.setPosition(100, 350);

    GameOption selectedOption = SNAKE;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                    case sf::Keyboard::Up:
                        selectedOption = static_cast<GameOption>((selectedOption - 1 + FIX_IT_FELIX + 1) % (FIX_IT_FELIX + 1));
                        break;
                    case sf::Keyboard::Down:
                        selectedOption = static_cast<GameOption>((selectedOption + 1) % (FIX_IT_FELIX + 1));
                        break;
                    case sf::Keyboard::Enter:
                        std::cout << "Selected option: ";
                        switch (selectedOption) {
                            case SNAKE:
                                std::cout << "Snake";
                                break;
                            case PACMAN:
                                std::cout << "Pacman";
                                break;
                            case NIBBLER:
                                std::cout << "Nibbler";
                                break;
                            case RTYPE:
                                std::cout << "R-Type";
                                break;
                            case RUNNER:
                                std::cout << "Runner";
                                break;
                            case FIX_IT_FELIX:
                                std::cout << "Fix it Felix Jr.";
                                break;
                        }
                        std::cout << std::endl;
                        break;
                    default:
                        break;
                }
            }
        }

        window.clear();

        snakeText.setFillColor(sf::Color::White);
        pacmanText.setFillColor(sf::Color::White);
        nibblerText.setFillColor(sf::Color::White);
        fixItFelixText.setFillColor(sf::Color::White);
        rtypetext.setFillColor(sf::Color::White);
        runnertext.setFillColor(sf::Color::White);

        switch (selectedOption) {
            case SNAKE:
                snakeText.setFillColor(sf::Color::Red);
                break;
            case PACMAN:
                pacmanText.setFillColor(sf::Color::Red);
                break;
            case NIBBLER:
                nibblerText.setFillColor(sf::Color::Red);
                break;
            case RTYPE:
                rtypetext.setFillColor(sf::Color::Red);
                break;
            case RUNNER:
                runnertext.setFillColor(sf::Color::Red);
                break;
            case FIX_IT_FELIX:
                fixItFelixText.setFillColor(sf::Color::Red);
                break;
        }

        window.draw(snakeText);
        window.draw(pacmanText);
        window.draw(nibblerText);
        window.draw(rtypetext);
        window.draw(runnertext);
        window.draw(fixItFelixText);

        window.display();
    }

    return 0;
}

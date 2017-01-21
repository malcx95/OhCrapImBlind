#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

#include <cAudio/cAudio.h>
#include <SFML/Graphics.hpp>

#include "level.hpp"


int main() {
    srand(time(nullptr));

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML window", sf::Style::Titlebar | sf::Style::Close);
    window.setMouseCursorVisible(false);

    std::cout << "Loading level" << std::endl;
    Level level;

    sf::Clock deltaClock;
    std::cout << "Starting main loop" << std::endl;
    while (window.isOpen())
    {
        sf::Time dt = deltaClock.restart();
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            default:
                break;
            }
        }

        const auto center = sf::Vector2i(WIDTH / 2, HEIGHT / 2);
        const auto current = sf::Mouse::getPosition(window);
        sf::Mouse::setPosition(center, window);
        level.turn_player((current - center).x);

        level.update(dt.asSeconds());

        // Clear screen
        window.clear();

        level.draw(&window);

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}

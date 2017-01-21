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
            if (event.type == sf::Event::Closed)
                window.close();
        }
        level.update(dt.asSeconds());

        // Clear screen
        window.clear();

        level.draw(&window);

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}
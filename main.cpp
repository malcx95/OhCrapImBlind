<<<<<<< HEAD
#include <SFML/Graphics.hpp>

#include "level.hpp"
#include <unistd.h>


int main() {
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

    Level level();

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // Clear screen
        window.clear();

        level.update();
        usleep(100000);

        // Update the window
        window.display();
    }
    return EXIT_SUCCESS;
}

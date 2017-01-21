#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <unistd.h>

#include <cAudio/cAudio.h>
#include <SFML/Graphics.hpp>

#include "level.hpp"

void init() {
    srand(time(NULL));
}

int generate_random_index(int last_index, unsigned int length) {
    // A negative last_index means that you don't want to skip any index
    if (last_index > 0) {
        int random_n = rand() % length;
        if (random_n >= last_index) {
            return random_n + 1;
        } else {
            return random_n;
        }
    }
    return rand() % (length - 1);
}

int change_lvl;

int main() {
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML window", sf::Style::Titlebar | sf::Style::Close);

    std::cout << "Loading level" << std::endl;

    float rot = 0.0f;

    Level level;
    // Start the game loop

    std::cout << "Starting main loop" << std::endl;

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

        // Move the audio source
        rot += 0.1f * 0.017453293f;
        float x = 5.0f * cosf(rot);
        float z = 5.0f * sinf(rot);

        level.update();


        // Clear screen
        window.clear();

        level.draw(&window);

        // Update the window
        window.display();
    }

    return EXIT_SUCCESS;
}

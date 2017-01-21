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

int main() {
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

    Level level;

    float rot = 0.0f;

    cAudio::IAudioManager* audio_mgr = cAudio::createAudioManager(true);
    if (!audio_mgr) {
        std::cerr << "ERROR: Could not create audio manager" << std::endl;
        exit(EXIT_FAILURE);
    }

    cAudio::IAudioSource* my_sound = audio_mgr->create("water", "audio/wood/wood1.ogg", true);
    if (!my_sound) {
        std::cerr << "ERROR: Could not load water.ogg" << std::endl;
        exit(EXIT_FAILURE);
    }

    cAudio::IListener* listener = audio_mgr->getListener();
    listener->setPosition(cAudio::cVector3(0, 0, 0));

    my_sound->play3d(cAudio::cVector3(0, 0, 0), 2.0f, true);
    my_sound->setVolume(1.0f);
    my_sound->setMinDistance(1.0f);
    my_sound->setMaxAttenuationDistance(100.0f);

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

        // Move the audio source
        rot += 0.1f * 0.017453293f;
        float x = 5.0f * cosf(rot);
        float z = 5.0f * sinf(rot);
        my_sound->move(cAudio::cVector3(x, 0.0f, z));

        // Clear screen
        window.clear();

        level.update();
        usleep(1000);

        // Update the window
        window.display();
    }

    cAudio::destroyAudioManager(audio_mgr);
    return EXIT_SUCCESS;
}

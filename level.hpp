#ifndef LEVEL
#define LEVEL

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cAudio/cAudio.h>

const float DEFAULT_PLAYER_X = 0.0;
const float DEFAULT_PLAYER_Y = 0.0;

const sf::Vector2<float> UP     = sf::Vector2<float>(0, 1.0);
const sf::Vector2<float> DOWN   = sf::Vector2<float>(0, -1.0);
const sf::Vector2<float> RIGHT  = sf::Vector2<float>(1.0, 0);
const sf::Vector2<float> LEFT   = sf::Vector2<float>(-1.0, 0);
const sf::Vector2<float> STILL  = sf::Vector2<float>(0, 0);

const std::string DEFAULT_MAP = "maps/map-default.png";
const std::string DEFAULT_AUDIO_MAP = "data/test_audio.json";

const sf::Color WOOD = sf::Color(0x60,0x32,0x00, 255);
const sf::Color GRAVEL = sf::Color(0x4b, 0x4b, 0x4b, 255);
const sf::Color GRASS = sf::Color(0x00, 0xff, 0x00, 255);
const sf::Color PUDDLE = sf::Color(0x00, 0x00, 0xff, 255);

struct AudioSource {

    sf::Vector2<float> pos;

    cAudio::IAudioSource* audio;

};

class Level {

    public:

        Level();

        sf::Vector2<float> get_player_pos() const;
        sf::Vector2<float> get_player_velocity() const;

        void update();

    private:

        std::vector<AudioSource> audio_sources;

        sf::Vector2<float> player_pos;
        sf::Vector2<float> player_velocity;

        float player_speed;

        sf::Image sound_map;

        cAudio::IAudioManager* audio_manager;

        /*
         * Reads keyboard input and updates the
         * player velocities.
         */
        void handle_input();

        /*
         * Sets the player velocity such that it doesn't
         * walk through a wall. For example, walking straight
         * into a wall would just set the velocity to (0, 0).
         */
        void handle_collisions();

        /*
         * Adds the player velocity scaled by the speed
         */
        void update_player_position();

        void load_audio_sources();
};

#endif /* ifndef LEVEL */

#ifndef LEVEL
#define LEVEL

#include <SFML/Graphics.hpp>

const float DEFAULT_PLAYER_X = 0.0;
const float DEFAULT_PLAYER_Y = 0.0;

const sf::Vector2<float> UP     = sf::Vector2<float>(0, 1.0);
const sf::Vector2<float> DOWN   = sf::Vector2<float>(0, -1.0);
const sf::Vector2<float> RIGHT  = sf::Vector2<float>(1.0, 0);
const sf::Vector2<float> LEFT   = sf::Vector2<float>(-1.0, 0);

class Level {

    public:

        Level();

        sf::Vector2<float> get_player_pos() const;
        sf::Vector2<float> get_player_velocity() const;

        void update();
    
    private:

        sf::Vector2<float> player_pos;
        sf::Vector2<float> player_velocity;

        float player_speed;

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
};

#endif /* ifndef LEVEL */

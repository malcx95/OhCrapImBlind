#ifndef LEVEL
#define LEVEL

#include <SFML/Graphics.hpp>

const float DEFAULT_PLAYER_X = 0.0;
const float DEFAULT_PLAYER_Y = 0.0;

class Level {

    public:

        Level();

        sf::Vector2<float> get_player_pos();
        sf::Vector2<float> get_player_speed();
    
    private:

        sf::Vector2<float> player_pos;
        sf::Vector2<float> player_speed;

};

#endif /* ifndef LEVEL */

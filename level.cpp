#include "level.hpp"

Level::Level() {
    this->player_pos = sf::Vector2<float>(DEFAULT_PLAYER_X, DEFAULT_PLAYER_Y);
    this->player_speed = sf::Vector2<float>(0, 0);
}

sf::Vector2<float> Level::get_player_pos() {
    return player_pos;
}

sf::Vector2<float> Level::get_player_speed() {
    return player_speed;
}

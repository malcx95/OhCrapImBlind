#include "level.hpp"

Level::Level() {
    this->player_pos = sf::Vector2<float>(DEFAULT_PLAYER_X, DEFAULT_PLAYER_Y);
    this->player_velocity = sf::Vector2<float>(0, 0);
    this->player_speed = 0;
}

sf::Vector2<float> Level::get_player_pos() const {
    return player_pos;
}

sf::Vector2<float> Level::get_player_velocity() const {
    return player_velocity;
}

void Level::update() {
    handle_input();
    handle_collisions();
}

void Level::handle_input() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        this->player_velocity = LEFT;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        this->player_velocity = RIGHT;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        this->player_velocity = UP;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        this->player_velocity = DOWN;
    }
}

void Level::handle_collisions() {
    // TODO implement
}

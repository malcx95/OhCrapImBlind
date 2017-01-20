#include "level.hpp"
#include <iostream>

Level::Level() {
    this->player_pos = sf::Vector2<float>(DEFAULT_PLAYER_X, DEFAULT_PLAYER_Y);
    this->player_velocity = sf::Vector2<float>(0, 0);
    this->player_speed = 1;
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
    update_player_position();
}

void Level::handle_input() {

    this->player_velocity = STILL;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        this->player_velocity += UP;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        this->player_velocity += LEFT;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        this->player_velocity += DOWN;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        this->player_velocity += RIGHT;
    }
}

void Level::handle_collisions() {
    // TODO implement
}

void Level::update_player_position() {
    this->player_pos += this->player_velocity * this->player_speed;
    std::cout << "X: " << player_pos.x << " Y: " << player_pos.y << std::endl;
}

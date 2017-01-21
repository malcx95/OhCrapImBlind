#include "level.hpp"
#include <iostream>
#include "json.hpp"
#include <fstream>

using namespace nlohmann;

Level::Level() {
    this->player_pos = sf::Vector2<float>(DEFAULT_PLAYER_X, DEFAULT_PLAYER_Y);
    this->player_velocity = sf::Vector2<float>(0, 0);
    this->player_speed = 1;
    if (!this->sound_map.loadFromFile(DEFAULT_MAP)) {
        std::cerr << "\"" << DEFAULT_MAP << "\" doesn't exist!" << std::endl;
    }
    load_audio_sources();
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
    /*sf::Vector2<float> next_pos = player_velocity * player_speed;
    sf::Color next_color = sound_map.getPixel(next_pos.x, next_pos.y);
    if (next_color == sf::Color::Black) {
        player_velocity.x = 0.f;
        player_velocity.y = 0.f;
    }*/
}

void Level::update_player_position() {
    this->player_pos += this->player_velocity * this->player_speed;
    std::cout << "X: " << player_pos.x << " Y: " << player_pos.y << std::endl;
}

void Level::load_audio_sources() {
    json json_data;
    std::ifstream file(DEFAULT_AUDIO_MAP);
    file >> json_data;
    file.close();

    for (auto source : json_data) {
        auto position = source[0];
        std::string file = source[1];
        
    }
}

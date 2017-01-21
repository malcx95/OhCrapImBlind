#include "level.hpp"
#include <iostream>
#include "json.hpp"
#include <fstream>

using namespace nlohmann;



Level::Level() {
    this->player_pos = sf::Vector2<float>(DEFAULT_PLAYER_X, DEFAULT_PLAYER_Y);
    this->player_velocity = sf::Vector2<float>(0, 0);
    this->player_speed = 1;

    std::cout << "Loading map texture" << std::endl;
    if (!this->sound_map.loadFromFile(DEFAULT_MAP)) {
        std::cerr << "\"" << DEFAULT_MAP << "\" doesn't exist!" << std::endl;
    }

    level_texture.loadFromImage(sound_map);
    level_sprite = sf::Sprite(level_texture);

    std::cout << "Loading audio" << std::endl;
    load_audio_sources();
}

Level::~Level() {
    cAudio::destroyAudioManager(this->audio_manager);
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
    sf::Vector2<float> next_pos = player_velocity * player_speed;
    sf::Color next_color = sound_map.getPixel(next_pos.x, next_pos.y);
    if (next_color == sf::Color::Black) {
        player_velocity.x = 0.f;
        player_velocity.y = 0.f;
        std::cout << "That there's a wall mate!" << std::endl;
    }
}

void Level::update_player_position() {
    this->player_pos += this->player_velocity * this->player_speed;
    std::cout << "X: " << player_pos.x << " Y: " << player_pos.y << std::endl;
}

void Level::load_audio_sources() {
    std::cout << "Loading " << DEFAULT_AUDIO_MAP << std::endl;
    json json_data;
    std::ifstream file(DEFAULT_AUDIO_MAP);
    file >> json_data;
    file.close();

    std::cout << "Initializing audio manager" << std::endl;
    this->audio_manager = cAudio::createAudioManager(true);

    int c = 0;
    for (auto source : json_data) {
        auto position = source[0];
        std::string file_name = source[1];

        std::cout << "Loading " << file_name << std::endl;
        cAudio::IAudioSource* sound = this->audio_manager->create(
            std::to_string(c).data(), file_name.data(), true
        );

        if (!sound) {
            std::cerr << "ERROR: Could not load " << file_name << std::endl;
            exit(EXIT_FAILURE);
        }

        AudioSource as = {
            sf::Vector2<float>(position[0], position[1]),
            sound
        };
        audio_sources.push_back(as);
    }
}

void Level::draw(sf::RenderTarget* target)
{
    target->draw(level_sprite);
}

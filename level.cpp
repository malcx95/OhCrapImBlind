#include "level.hpp"
#include <iostream>
#include "json.hpp"
#include <fstream>
#include "util.hpp"
#include <math.h>

using namespace nlohmann;

Level::Level() {
    this->in_dev_mode = true;

    this->player_pos = sf::Vector2<float>(DEFAULT_PLAYER_X, DEFAULT_PLAYER_Y);
    this->player_velocity = sf::Vector2<float>(0, 0);
    this->player_speed = 1;

    this->step_delay = 0.5f;
    this->step_timer = 0;

    std::cout << "Loading map texture" << std::endl;
    if (!this->sound_map.loadFromFile(DEFAULT_MAP)) {
        std::cerr << "\"" << DEFAULT_MAP << "\" doesn't exist!" << std::endl;
    }

    this->level_texture.loadFromImage(this->sound_map);
    this->level_sprite = sf::Sprite(this->level_texture);

    std::cout << "Loading audio" << std::endl;
    load_json_data();

    play_audio_sources();

    ground = new Ground(this->audio_manager);

    this->load_collision_audio();
}

Level::~Level() {
    delete ground;
    cAudio::destroyAudioManager(this->audio_manager);
}

sf::Vector2<float> Level::get_player_pos() const {
    return player_pos;
}

sf::Vector2<float> Level::get_player_velocity() const {
    return player_velocity;
}

void Level::play_audio_sources() {
    for (AudioSource s : this->audio_sources) {
        s.audio->play3d(util::sf_to_caudio_vect(s.pos), 10.0, true);
    }
}

void Level::update(float dt) {
    handle_input();
    handle_collisions();
    update_player_position();
    handle_steps(dt);
    if (has_reached_goal()) {
      std::cout << "Reached Goal" << std::endl;
    }
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

Mat::Material Level::ground_under_player() {
    return Mat::WOOD;
}

void Level::handle_steps(float dt) {
    if (this->step_timer <= 0) {
        this->step_timer += this->step_delay;
        ground->play_random_step(ground_under_player());
    }

    if (this->player_velocity != STILL) {
        this->step_timer -= dt;
    }
}

bool Level::has_reached_goal() {
    return GOAL_RADIUS >= util::distance(this->player_pos, this->goal_position);
}

void Level::handle_collisions() {
    // TODO implement
    sf::Vector2<float> next_pos = player_pos + player_velocity * player_speed;
    if ( next_pos.x <= WIDTH && next_pos.x >= 0 && next_pos.y <= HEIGHT && next_pos.y >= 0 ) {
        sf::Color next_color = sound_map.getPixel(next_pos.x, next_pos.y);
        if (next_color == sf::Color::Black) {
            player_velocity.x = 0.f;
            player_velocity.y = 0.f;
            //std::cout << "That there's a wall mate!" << std::endl;
            this->play_collision_sound();
        }
    } else {
        player_velocity.x = 0.f;
        player_velocity.y = 0.f;
        std::cout << "Don't go off the map you idiot!" << std::endl;
        std::cout << "predicted X: " << next_pos.x << " predicted Y: " << next_pos.y << std::endl;
    }
}

void Level::update_player_position() {
    this->player_pos += this->player_velocity * this->player_speed;

    // update the audio listener
    this->listener->setPosition(util::sf_to_caudio_vect(this->player_pos));
    sf::Vector2<float> v = this->player_velocity * this->player_speed;
    this->listener->setVelocity(util::sf_to_caudio_vect(v));

    //std::cout << "X: " << player_pos.x << " Y: " << player_pos.y << std::endl;
}

void Level::load_json_data() {
    std::cout << "Loading " << DEFAULT_AUDIO_MAP << std::endl;
    json json_data;
    std::ifstream file(DEFAULT_AUDIO_MAP);
    file >> json_data;
    file.close();

    std::cout << "Initializing audio manager" << std::endl;
    this->audio_manager = cAudio::createAudioManager(true);
    this->listener = this->audio_manager->getListener();

    int c = 0;

    // get the goal data from the json file
    this->goal_position = sf::Vector2<float>(json_data["goal"][0], json_data["goal"][1]);

    if (!goal_texture.loadFromFile(GOAL_SPRITE)) {
      std::cerr << "\"" << GOAL_SPRITE << "\" doesn't exist!" << std::endl;
    }
    this->goal_texture.loadFromFile(GOAL_SPRITE);
    this->goal_sprite = sf::Sprite(this->goal_texture);
    goal_sprite.setPosition(this->goal_position.x, this->goal_position.y);
    goal_sprite.setOrigin(GOAL_RADIUS, GOAL_RADIUS);

    auto player_positions = json_data["start_positions"];
    auto selected_position = player_positions[rand() % player_positions.size()];
    this->player_pos = sf::Vector2<float>(selected_position[0], selected_position[1]);
    std::cout << selected_position << std::endl;

    // load the audio sources sources
    auto audio_data = json_data["audio"];
    for (auto source : audio_data) {
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

void Level::load_collision_audio(){
    this->wall_collision_sources.push_back(this->audio_manager->create(
                    "why do you need a name", "../audio/wood.ogg", false
                ));
}

void Level::draw(sf::RenderTarget* target)
{
    target->draw(level_sprite);
    target->draw(goal_sprite);

    if (this->in_dev_mode) {
        debug_draw_player(target);
        debug_draw_audio_sources(target);
    }
}

void Level::debug_draw_player(sf::RenderTarget* target) {
    float player_x = this->player_pos.x;
    float player_y = this->player_pos.y;

    sf::Vertex hline[] = {
        sf::Vertex(sf::Vector2f(player_x - 5, player_y), sf::Color::Red),
        sf::Vertex(sf::Vector2f(player_x + 5, player_y), sf::Color::Red)
    };

    sf::Vertex vline[] = {
        sf::Vertex(sf::Vector2f(player_x, player_y - 5), sf::Color::Red),
        sf::Vertex(sf::Vector2f(player_x, player_y + 5), sf::Color::Red)
    };

    target->draw(hline, 2, sf::Lines);
    target->draw(vline, 2, sf::Lines);
}

void Level::debug_draw_audio_sources(sf::RenderTarget* target) {
    for (AudioSource s : this->audio_sources) {
        float x = s.pos.x;
        float y = s.pos.y;
        sf::Vertex hline[] = {
            sf::Vertex(sf::Vector2f(x - 5, y), sf::Color::Green),
            sf::Vertex(sf::Vector2f(x + 5, y), sf::Color::Green)
        };

        sf::Vertex vline[] = {
            sf::Vertex(sf::Vector2f(x, y - 5), sf::Color::Green),
            sf::Vertex(sf::Vector2f(x, y + 5), sf::Color::Green)
        };

        target->draw(hline, 2, sf::Lines);
        target->draw(vline, 2, sf::Lines);
    }
}

void Level::play_collision_sound() {
    auto selected_sound = rand() % this->wall_collision_sources.size();

    this->wall_collision_sources[selected_sound]->play2d(false);

    std::cout << "Wall collision" << std::endl;
}

#include "level.hpp"
#include <iostream>
#include "json.hpp"
#include <fstream>
#include "util.hpp"

using namespace nlohmann;

Level::Level() {
    this->in_dev_mode = true;

    this->player_pos = sf::Vector2<float>(DEFAULT_PLAYER_X, DEFAULT_PLAYER_Y);
    this->player_velocity = sf::Vector2<float>(0, 0);
    this->player_speed = 1;
    
    map_path = DEFAULT_MAP;

    std::cout << "Loading audio" << std::endl;
    load_json_data();
    play_audio_sources();
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

void Level::play_audio_sources() {
    for (AudioSource s : this->audio_sources) {
        s.audio->play3d(util::sf_to_caudio_vect(s.pos), s.attenuation, true);
    }
}

void Level::update() {
    handle_input();
    handle_collisions();
    update_player_position();
    handle_steps(dt);
    if (has_reached_goal()) {
      std::cout << "Reached Goal" << std::endl;
    }
}

void Level::handle_input() {

    bool change_lvl = false;
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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
        change_lvl = true;
    }
    else {
        changed_level = false;
        change_lvl = false;
    }
    
    if (change_lvl && !changed_level) {
      change();
      changed_level = true;
    }
}

bool Level::has_reached_goal() {
    return GOAL_RADIUS >= util::distance(this->player_pos, this->goal_position);
}

void Level::handle_collisions() {
    sf::Vector2<float> next_pos = player_pos + player_velocity * player_speed;
    if ( next_pos.x <= WIDTH && next_pos.x >= 0 && next_pos.y <= HEIGHT && next_pos.y >= 0 ) {
        sf::Color next_color = sound_map.getPixel(next_pos.x, next_pos.y);
        if (next_color == sf::Color::Black) {
            player_velocity.x = 0.f;
            player_velocity.y = 0.f;
            std::cout << "That there's a wall mate!" << std::endl;
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

    std::cout << "level number: "<< level_num << std::endl;
    //get map path from json file
    map_path = json_data["map_list"][level_num]["path"];
    
    std::cout << "Loading map texture" << std::endl;
    if (!this->sound_map.loadFromFile(map_path)) {
        std::cerr << "\"" << map_path << "\" doesn't exist!" << std::endl;
    }
    
    this->level_texture.loadFromImage(this->sound_map);
    this->level_sprite = sf::Sprite(this->level_texture);
    
    
    //get player start position from json file
    auto start_pos = json_data["map_list"][level_num]["start_position"];
    player_pos.x = start_pos[0];
    player_pos.y = start_pos[1];
    std::cout << "start_position: " << player_pos.x <<  " " << player_pos.y << std::endl;
        
    
    // get the goal data from the json file + handle goal sprite
    this->goal_position = sf::Vector2<float>(json_data["map_list"][level_num]["goal"][0], 
                                            json_data["map_list"][level_num]["goal"][1]);

    if (!goal_texture.loadFromFile(GOAL_SPRITE)) {
      std::cerr << "\"" << GOAL_SPRITE << "\" doesn't exist!" << std::endl;
    }
    
    this->goal_texture.loadFromFile(GOAL_SPRITE);
    this->goal_sprite = sf::Sprite(this->goal_texture);
    std::cout << "goal_position: " << goal_position.x << " "<< goal_position.y << std::endl;
    goal_sprite.setPosition(goal_position.x, goal_position.x);
    goal_sprite.setPosition(this->goal_position.x, this->goal_position.y);
    goal_sprite.setOrigin(GOAL_RADIUS, GOAL_RADIUS);
    
    
    
    //init audio manager
    std::cout << "Initializing audio manager" << std::endl;
    this->audio_manager = cAudio::createAudioManager(true);
    this->listener = this->audio_manager->getListener();

    int c = 0;
    
    audio_sources.clear();

    // load the audio sources sources
    auto audio_data = json_data["map_list"][level_num]["audio"];
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
            sound, source[2]
        };
        audio_sources.push_back(as);
    }
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

void Level::change() {
    std::cout << " CHANGING LEVEL \n\n\n";
    level_num ++;
    load_json_data();//WOW SUCH FUNCTION
}


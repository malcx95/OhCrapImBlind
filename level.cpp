#include "level.hpp"
#include <iostream>
#include "json.hpp"
#include <fstream>
#include "util.hpp"
#include <math.h>
#include <cstdlib>

using namespace nlohmann;

Level::Level() {
    this->in_dev_mode = true;

    this->player_pos = sf::Vector2<float>(DEFAULT_PLAYER_X, DEFAULT_PLAYER_Y);
    this->player_velocity = sf::Vector2<float>(0, 0);
    this->player_speed = 50;

    std::cout << "Loading map texture" << std::endl;
    if (!this->sound_map.loadFromFile(DEFAULT_MAP)) {
        std::cerr << "\"" << DEFAULT_MAP << "\" doesn't exist!" << std::endl;
    }

    this->level_texture.loadFromImage(this->sound_map);
    this->level_sprite = sf::Sprite(this->level_texture);
    
    map_path = DEFAULT_MAP;

    std::cout << "Loading audio" << std::endl;
    load_json_data();
    play_audio_sources();

    ground = new Ground(this->audio_manager);

    this->load_collision_audio();

    this->car_engine = this->audio_manager->create(CAR_ENGINE.data(), 
            CAR_ENGINE.data(), false);
    this->car_honk = this->audio_manager->create(CAR_HONK.data(), 
            CAR_HONK.data(), false);
    this->current_car = nullptr;
    
    this->step_delay = 0.5f;
    this->step_timer = 0;
    this->roads = std::vector<CarRoad>();
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
  std::cout << "trying to play audio sources\n";
    for (AudioSource s : this->audio_sources) {
        s.audio->play3d(util::sf_to_caudio_vect(s.pos), s.attenuation, true);
    }
}

void Level::update(float dt) {
    handle_input();
    handle_collisions(dt);
    update_player_position(dt);
    handle_steps(dt);
    update_car();
    maybe_spawn_car();
    if (has_reached_goal()) {
        std::cout << "Reached Goal" << std::endl;
        change();
    }
}

void Level::maybe_spawn_car() {
    if (this->current_car != nullptr) {
        int r = rand() % CAR_SPAWN_RATE;

        if (r == 0) {
            // select a road to spawn a car on
            int i = rand() % this->roads.size();
            CarRoad road = this->roads[i];
            
            float d = (float)(rand() % 3) - 1;

            sf::Vector2<float> pos;
            sf::Vector2<float> vel;
            
            if (road.direction == HORIZONTAL) {
                vel = sf::Vector2<float>(d, 0);
                if (d == -1.0) {
                    pos = sf::Vector2<float>(WIDTH - 1, road.pos);
                } else {
                    pos = sf::Vector2<float>(0, road.pos);
                }
            } else {
                vel = sf::Vector2<float>(0, d);
                if (d == -1.0) {
                    pos = sf::Vector2<float>(road.pos, HEIGHT - 1);
                } else {
                    pos = sf::Vector2<float>(road.pos, 0);
                }
            }

            this->current_car = new Car(pos, vel, 
                    this->car_engine, this->car_honk);
            this->current_car->start();
        }
    }
}

void Level::update_car() {
    if (this->current_car != nullptr) {
        this->current_car->update_position();
        if (this->current_car->out_of_bounds(WIDTH - 1, HEIGHT - 1)) {
            this->current_car->stop();
            delete this->current_car;
            this->current_car = nullptr;
        } else {
            this->current_car->honk_if_close_to(
                    this->player_pos, HONKING_DISTANCE);
        }
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

void Level::handle_collisions(float dt) {
    sf::Vector2<float> next_pos = player_pos + player_velocity * player_speed * dt;
    if ( next_pos.x <= WIDTH && next_pos.x >= 0 && next_pos.y <= HEIGHT && next_pos.y >= 0 ) {
        sf::Color next_color = sound_map.getPixel(next_pos.x, next_pos.y);
        if (next_color == sf::Color::Black) {
            player_velocity.x = 0.f;
            player_velocity.y = 0.f;
            std::cout << "That there's a wall mate!" << std::endl;
            this->play_collision_sound();
        }
    } else {
        player_velocity.x = 0.f;
        player_velocity.y = 0.f;
        std::cout << "Don't go off the map you idiot!" << std::endl;
        std::cout << "predicted X: " << next_pos.x << " predicted Y: " << next_pos.y << std::endl;
    }
}

void Level::update_player_position(float dt) {
    sf::Vector2<float> v = this->player_velocity * this->player_speed * dt;
    this->player_pos += v;

    // update the audio listener
    this->listener->setPosition(util::sf_to_caudio_vect(this->player_pos));
    //sf::Vector2<float> v = this->player_velocity * this->player_speed;
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
    
    //reset player fields
    this->player_pos = sf::Vector2<float>(DEFAULT_PLAYER_X, DEFAULT_PLAYER_Y);
    this->player_velocity = sf::Vector2<float>(0, 0);
        
    //get player start position from json file
    auto start_pos = json_data["map_list"][level_num]["start_positions"][0];
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
        auto position_list = source[0];
        std::string file_name = source[1];

        std::cout << "Loading " << file_name << std::endl;
        cAudio::IAudioSource* sound = this->audio_manager->create(
            std::to_string(c).data(), file_name.data(), false 
        );

        if (!sound) {
            std::cerr << "ERROR: Could not load " << file_name << std::endl;
            exit(EXIT_FAILURE);
        }

        sf::Vector2<float> position(position_list[0], position_list[1]);
        auto sprite_paths = source[3];
        std::vector<std::pair<sf::Texture, sf::Sprite>> sprites;
        for (auto path : sprite_paths)
        {
            sf::Texture texture;
            texture.loadFromFile(path);
            sf::Sprite sprite(texture);
            sprite.setPosition(position);

            sprites.push_back(std::pair<sf::Texture, sf::Sprite>(texture, sprite));
        }

        AudioSource as = {
            position,
            sound, 
            source[2],
            sprites
        };
        audio_sources.push_back(as);
    }

    this->roads.clear();

    if (json_data["map_list"][level_num].count("cars") != 0) {
        for (auto car : json_data["map_list"][level_num]["cars"]) {
            CarRoad road;
            if (car[0] == "horizontal") {
                road.direction = HORIZONTAL;
            } else {
                road.direction = VERTICAL;
            }
            road.pos = car[1];
            this->roads.push_back(road);
        }
    }
}

void Level::load_collision_audio(){
    this->wall_collision_sources.push_back(this->audio_manager->create(
                    "collision1", "../audio/walls/collision1.ogg", false
                ));
    this->wall_collision_sources.push_back(this->audio_manager->create(
                    "collision2", "../audio/walls/collision2.ogg", false
                ));
    this->wall_collision_sources.push_back(this->audio_manager->create(
                    "collision3", "../audio/walls/collision3.ogg", false
                ));
    this->wall_collision_sources.push_back(this->audio_manager->create(
                    "collision4", "../audio/walls/collision4.ogg", false
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


void Level::change() {
    std::cout << " CHANGING LEVEL \n\n\n";
    level_num ++;
    load_json_data();//WOW SUCH FUNCTION
    
    play_audio_sources();

    ground = new Ground(this->audio_manager);

    this->load_collision_audio();

    this->car_engine = this->audio_manager->create(CAR_ENGINE.data(), 
            CAR_ENGINE.data(), false);
    this->car_honk = this->audio_manager->create(CAR_HONK.data(), 
            CAR_HONK.data(), false);
    this->current_car = nullptr;
    
    this->step_delay = 0.5f;
    this->step_timer = 0;
}


Mat::Material Level::ground_under_player() {
    sf::Color ground_col = sound_map.getPixel(player_pos.x, player_pos.y);
    if (ground_col == WOOD) return Mat::WOOD;
    if (ground_col == GRAVEL) return Mat::GRAVEL;
    if (ground_col == GRASS) return Mat::GRASS;
    return Mat::PUDDLE;    
}

void Level::handle_steps(float dt) {
    if (this->step_timer <= 0) {
        this->step_timer += this->step_delay;
        ground->play_random_step(ground_under_player());
        std::cout << "step dt =" << dt << std::endl;
    }

    if (this->player_velocity != STILL) {
        this->step_timer -= dt;
    }
}

void Level::play_collision_sound() {
    auto selected_sound = rand() % this->wall_collision_sources.size();

    this->wall_collision_sources[selected_sound]->play2d(false);

    std::cout << "Wall collision" << std::endl;
    
}

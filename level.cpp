#include "level.hpp"
#include <iostream>
#include "json.hpp"
#include <fstream>
#include "util.hpp"
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <algorithm>

using namespace nlohmann;

Level::Level() {
    this->in_dev_mode = false;

    this->player_pos = sf::Vector2<float>(DEFAULT_PLAYER_X, DEFAULT_PLAYER_Y);
    this->player_velocity = sf::Vector2<float>(0, 0);
    this->player_speed = PLAYER_SPEED;

    this->available_cars = std::vector<Car*>();
    this->cars_in_use = std::vector<Car*>();

    //init audio manager
    this->audio_manager = cAudio::createAudioManager(true);
    this->audio_manager->setDopplerFactor(DOPPLER_FACTOR);
    this->listener = this->audio_manager->getListener();

#if CAUDIO_EFX_ENABLED == 1
    auto effects = audio_manager->getEffects();
    this->lp_filter = effects->createFilter();
    this->lp_filter->setHighFrequencyVolume(0.1f);
    //this->lp_filter->setLowFrequencyVolume(0.1f);
    this->lp_filter->setType(cAudio::EFT_LOWPASS);
#endif

    std::cout << "Loading world from Json" << std::endl;

    map_path = DEFAULT_MAP;

    load_swears_and_cars();

    std::cout << "Loading audio" << std::endl;
    load_json_data();
    play_audio_sources();

    this->ground = new Ground(this->audio_manager);

    this->load_collision_audio();

    this->step_delay = 0.5f;
    this->step_timer = 0;

    this->car_timer = 0;
}

Level::~Level() {
    cAudio::destroyAudioManager(this->audio_manager);
    for (Car* car : this->available_cars) {
        delete car;
    }
    for (Car* car : this->cars_in_use) {
        delete car;
    }
}

sf::Vector2<float> Level::get_player_pos() const {
    return player_pos;
}

sf::Vector2<float> Level::get_player_velocity() const {
    return player_velocity;
}

void Level::load_swears_and_cars() {
    // Create a car for each swear
    for (int i = 0; i < NUM_SWEARS; ++i) {
        cAudio::IAudioSource* swear = this->audio_manager->create(
                SWEARS[i].data(),
                SWEARS[i].data(), false);

        cAudio::IAudioSource* car_engine =
            this->audio_manager->create(CAR_ENGINE.data(),
                CAR_ENGINE.data(), false);

        cAudio::IAudioSource* car_honk = this->audio_manager->create(
                CAR_HONK.data(),
                CAR_HONK.data(), false);

        Car* c = new Car(car_engine, car_honk, swear);
        this->available_cars.push_back(c);
    }
}

void Level::play_audio_sources() {
    std::cout << "trying to play audio sources\n";
    for (AudioSource s : this->audio_sources) {
        //s.audio->play3d(util::sf_to_caudio_vect(s.pos), s.attenuation, true);
        for(auto sound : s.audio)
        {
            sound->play3d(util::sf_to_caudio_vect(s.pos), s.attenuation, true);
        }
    }
}

void Level::update(float dt) {
    handle_input();
    handle_collisions(dt);
    update_player_position(dt);
    handle_steps(dt);
    update_cars(dt);
    maybe_spawn_car(dt);
    if (has_reached_goal()) {
        std::cout << "Reached Goal" << std::endl;
        change(true);
    }

    for (auto& source : this->audio_sources)
    {
        source.update(dt);
    }

#if CAUDIO_EFX_ENABLED == 1
    handle_night_club_fx();
#endif

    time_since_collision_sound += dt;
}

#if CAUDIO_EFX_ENABLED == 1
void Level::handle_night_club_fx() {
    auto player_x = this->player_pos.x;
    auto player_y = this->player_pos.y;

    const auto DOOR_WIDTH = 10;

    for (auto club : this->night_clubs) {
        const auto club_x = club->getPosition().x;
        const auto club_y = club->getPosition().y;

        if (abs(player_x - club_x) < DOOR_WIDTH &&
            abs(player_y > club_y)) {
            club->attachFilter(this->lp_filter);
        } else {
            club->removeFilter();
        }
    }
}
#endif

void Level::maybe_spawn_car(float dt) {
    if (this->available_cars.size() > 0 && this->roads.size() > 0) {
        this->car_timer -= dt;

        if (car_timer < 0) {
            // select the emptiest road to spawn a car on
            size_t min_index = 0;
            int min = this->cars_in_use.size();
            for (size_t i = 0; i < this->roads.size(); ++i) {
                int num_cars = this->roads[i].num_cars;
                if (num_cars < min) {
                    min = num_cars;
                    min_index = i;
                }
            }

            RoadOrientation orientation =
                this->roads[min_index].orientation;
            float road_pos = this->roads[min_index].pos;
            int dir = this->roads[min_index].dir;

            this->roads[min_index].num_cars++;

            sf::Vector2<float> pos;
            sf::Vector2<float> vel;

            if (orientation == HORIZONTAL) {
                if (dir == 0) {
                    vel = sf::Vector2<float>(-1, 0);
                    pos = sf::Vector2<float>(CAR_DOMAIN_WIDTH, road_pos);
                } else {
                    vel = sf::Vector2<float>(1, 0);
                    pos = sf::Vector2<float>(-(CAR_DOMAIN_HEIGHT / 2), road_pos);
                }
            } else {
                if (dir == 0) {
                    vel = sf::Vector2<float>(0, -1);
                    pos = sf::Vector2<float>(road_pos, CAR_DOMAIN_HEIGHT);
                } else {
                    vel = sf::Vector2<float>(0, 1);
                    pos = sf::Vector2<float>(road_pos,
                            -(CAR_DOMAIN_HEIGHT / 2));
                }
            }

            std::random_shuffle(
                    this->available_cars.begin(),
                    this->available_cars.end());
            Car* car = this->available_cars.back();
            this->available_cars.pop_back();
            this->cars_in_use.push_back(car);
            car->set_road_index(min_index);
            car->start(pos, vel * CAR_SPEED);

            this->car_timer = CAR_SPAWN_DELAY;
            std::cout << "ROADS: " << std::endl;
            for (CarRoad r : this->roads) {
                std::cout << r.num_cars << std::endl;
            }
        }
    }
}

void Level::update_cars(float dt) {
    for (size_t i = 0; i < this->cars_in_use.size(); ++i) {
        Car* car = this->cars_in_use[i];
        car->update_position(dt);
        if (car->out_of_bounds(CAR_DOMAIN_WIDTH, CAR_DOMAIN_HEIGHT)) {
            // remove the car from the cars in use and put it
            // back to the available cars
            car->stop();
            this->roads[car->get_road_index()].num_cars--;
            this->cars_in_use.erase(this->cars_in_use.begin() + i);
            this->available_cars.push_back(car);
        } else {
            car->honk_if_close_to(
                    this->player_pos, HONKING_DISTANCE);
            car->swear_if_close_to(
                    this->player_pos, SWEAR_DISTANCE);
            if (car->collides_with(player_pos)) {
                play_collision_sound();
                splash_you_died();
            }
        }
    }
}

void Level::handle_input() {

    bool change_lvl = false;
    bool go_to_next = true;

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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::N) || sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
            go_to_next = false;
        change_lvl = true;
    }
    else {
        changed_level = false;
        change_lvl = false;
    }

    if (change_lvl && !changed_level) {
      change(go_to_next);
      changed_level = true;
    }
     if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
      std::cout << "x: " << this->player_pos.x << " y: " << this->player_pos.y << std::endl;
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

    std::string pretty_path = json_data["map_list"][level_num]["fancy_path"];
    if(!this->pretty_texture.loadFromFile(pretty_path))
    {
        std::cerr << "\"" << pretty_path << "\" doesn't exist!" << std::endl;
    }
    this->pretty_sprite.setTexture(this->pretty_texture);

    //reset player fields
    this->player_pos = sf::Vector2<float>(DEFAULT_PLAYER_X, DEFAULT_PLAYER_Y);
    this->player_velocity = sf::Vector2<float>(0, 0);

    //get player start position from json file
    auto start_pos_index = rand() % json_data["map_list"][level_num]["start_positions"].size();
    auto start_pos = json_data["map_list"][level_num]["start_positions"][start_pos_index];
    player_pos.x = start_pos[0];
    player_pos.y = start_pos[1];
    std::cout << "start_position: " << player_pos.x <<  " " << player_pos.y << std::endl;


    // get the goal data from the json file + handle goal sprite + handle goal sound
    this->goal_position = sf::Vector2<float>(json_data["map_list"][level_num]["goal"][0],
                                            json_data["map_list"][level_num]["goal"][1]);

    if (!goal_texture.loadFromFile(GOAL_SPRITE)) {
        std::cerr << "\"" << GOAL_SPRITE << "\" doesn't exist!" << std::endl;
    }

    this->goal_sprite = sf::Sprite(this->goal_texture);
    std::cout << "goal_position: " << goal_position.x << " "<< goal_position.y << std::endl;
    goal_sprite.setPosition(goal_position.x, goal_position.x);
    goal_sprite.setPosition(this->goal_position.x, this->goal_position.y);
    goal_sprite.setOrigin(GOAL_RADIUS, GOAL_RADIUS);

    int c = 0;

    audio_sources.clear();

    // load the audio sources sources
    auto audio_data = json_data["map_list"][level_num]["audio"];
    for (auto source : audio_data) {
        auto position_list = source[0];
        auto file_names = source[1];

        std::vector<cAudio::IAudioSource*> sounds;

        for (auto file_name : file_names)
        {
            std::string file_name_string = file_name;

            std::cout << "Loading " << file_name_string << std::endl;
            cAudio::IAudioSource* sound = this->audio_manager->create(
                std::to_string(c).data(), file_name_string.data(), true
            );

            if (!sound) {
                std::cerr << "ERROR: Could not load " << file_name_string << std::endl;
                exit(EXIT_FAILURE);
            }
            sound->setVolume(2);
            sounds.push_back(sound);

            if (file_name_string == "../audio/InTheClub.ogg") {
                night_clubs.push_back(sound);
#if CAUDIO_EFX_ENABLED == 1
                sound->attachFilter(this->lp_filter);
#endif
            }
        }

        //load and place sprites
        sf::Vector2<float> position(position_list[0], position_list[1]);
        auto sprite_paths = source[3];
        std::vector<sf::Sprite> sprites;
        std::vector<sf::Texture*> textures;
        for (auto path : sprite_paths)
        {
            sf::Texture* texture = new sf::Texture();
            if (!texture->loadFromFile(path))
            {
                std::cout << "Failed to load texture\"" << path << "\"" << std::endl;
            }
            sf::Sprite sprite;
            sprite.setOrigin(25, 25);
            sprite.setPosition(position);

            sprite.setTexture(*texture);
            sprites.push_back(sprite);
            textures.push_back(texture);
        }

        float play_rate = 0;
        if(source.size() > 4)
        {
            play_rate = source[4];
        }

        AudioSource as = {
            position,
            sounds,
            source[2],
            textures,
            sprites,
            0,
            0.0,
            play_rate
        };
        audio_sources.push_back(as);
    }

    this->roads.clear();

    if (json_data["map_list"][level_num].count("cars") != 0) {
        for (auto r : json_data["map_list"][level_num]["cars"]) {
            CarRoad road;
            if (r[0] == "horizontal") {
                road.orientation = HORIZONTAL;
                std::cout << "Added horizontal road" << std::endl;
            } else {
                road.orientation = VERTICAL;
                std::cout << "Added vertical road" << std::endl;
            }
            road.dir = r[1];
            road.pos = r[2];
            road.num_cars = 0;
            this->roads.push_back(road);
        }
    }
}

void Level::load_collision_audio() {
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

    this->wall_collision_voices.push_back(this->audio_manager->create(
                    "collision1", "../audio/walls/voice1.ogg", false
                ));
    this->wall_collision_voices.push_back(this->audio_manager->create(
                    "collision2", "../audio/walls/voice2.ogg", false
                ));
    this->wall_collision_voices.push_back(this->audio_manager->create(
                    "collision3", "../audio/walls/voice3.ogg", false
                ));
}

void Level::draw(sf::RenderTarget* target)
{
    target->draw(pretty_sprite);
    target->draw(goal_sprite);

    if (this->in_dev_mode) {
        debug_draw_player(target);
        debug_draw_audio_sources(target);
        debug_draw_cars(target);
    }

    for (auto source : this->audio_sources)
    {
        source.draw(target);
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

void Level::debug_draw_cars(sf::RenderTarget* target) {
    for (Car* c : this->cars_in_use) {
        float car_x = c->get_position().x;
        float car_y = c->get_position().y;

        sf::Vertex hline[] = {
            sf::Vertex(sf::Vector2f(car_x - CAR_WIDTH, car_y),
                    sf::Color::Blue),
            sf::Vertex(sf::Vector2f(car_x + CAR_WIDTH, car_y),
                    sf::Color::Blue)
        };

        sf::Vertex vline[] = {
            sf::Vertex(sf::Vector2f(car_x, car_y - CAR_WIDTH),
                    sf::Color::Blue),
            sf::Vertex(sf::Vector2f(car_x, car_y + CAR_WIDTH),
                    sf::Color::Blue)
        };

        target->draw(hline, 2, sf::Lines);
        target->draw(vline, 2, sf::Lines);
    }

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


void Level::change(bool go_to_next) {
    std::cout << " CHANGING LEVEL \n\n\n";
    if (go_to_next)
        level_num ++;

    // Stop the cars in use
    while (this->cars_in_use.size() > 0) {
        Car* c = this->cars_in_use.back();
        this->cars_in_use.pop_back();
        c->stop();
        this->available_cars.push_back(c);
    }
    for (AudioSource& source : audio_sources) {
      for (auto track : source.audio)
      {
          if (track->isPlaying())
            track->stop();
      }
    }
    load_json_data();

    play_audio_sources();

    ground = new Ground(this->audio_manager);

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
    if (time_since_collision_sound > COLLISION_SOUND_INTERVAL)
    {
        auto selected_sound = rand() % this->wall_collision_sources.size();
        this->wall_collision_sources[selected_sound]->play2d(false);

        selected_sound = rand() % this->wall_collision_voices.size();
        this->wall_collision_voices[selected_sound]->play2d(false);

        time_since_collision_sound = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
//              Audio source methods
////////////////////////////////////////////////////////////////////////////////
void AudioSource::draw(sf::RenderTarget* target)
{
    target->draw(sprites[current_sprite]);
}
void AudioSource::update(float dt)
{
    last_switch += dt;
    if(last_switch > AUDIO_SOURCE_ANIMATION_SPEED)
    {
        current_sprite = (current_sprite + 1) % sprites.size();
        last_switch = 0;
    }
}


void Level::splash_you_died() {
    //change texture
    std::cout << "Loading death splashscreen" << std::endl;
    if (!this->sound_map.loadFromFile("../splash/you_died.png")) {
        std::cerr << "\"" << map_path << "\" doesn't exist!" << std::endl;
    }

    this->level_texture.loadFromImage(this->sound_map);
    this->level_sprite = sf::Sprite(this->level_texture);
    this->pretty_sprite.setTexture(this->level_texture);


    //stop sounds
    for (AudioSource& source : audio_sources) {
      for (auto track : source.audio)
      {
          if (track->isPlaying())
            track->stop();
      }
    }

}

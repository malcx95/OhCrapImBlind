#ifndef LEVEL_H
#define LEVEL_H

#include "ground.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cAudio/cAudio.h>
#include "car.hpp"
#include <memory.h>

const float DEFAULT_PLAYER_X = 0.0;
const float DEFAULT_PLAYER_Y = 0.0;

const float PLAYER_TURN_SPEED = 0.1f;

const sf::Vector2<float> UP     = sf::Vector2<float>(0, -1.0);
const sf::Vector2<float> DOWN   = sf::Vector2<float>(0, 1.0);
const sf::Vector2<float> RIGHT  = sf::Vector2<float>(1.0, 0);
const sf::Vector2<float> LEFT   = sf::Vector2<float>(-1.0, 0);
const sf::Vector2<float> STILL  = sf::Vector2<float>(0, 0);

const int GOAL_RADIUS           = 50;

const std::string CAR_ENGINE = "../audio/car.ogg";
const std::string CAR_HONK = "../audio/horn.ogg";

const std::string DEFAULT_MAP = "../maps/map-default.png";
const std::string DEFAULT_AUDIO_MAP = "../data/test_audio.json";
const std::string GOAL_SPRITE = "../assets/goal.png";
const std::string ARROW_SPRITE = "../assets/arrow.png";

const sf::Color WOOD = sf::Color(0x60,0x32,0x00, 255);
const sf::Color GRAVEL = sf::Color(0x4b, 0x4b, 0x4b, 255);
const sf::Color GRASS = sf::Color(0x00, 0xff, 0x00, 255);
const sf::Color PUDDLE = sf::Color(0x00, 0x00, 0xff, 255);

const int PLAYER_SPEED = 60;

const int WIDTH = 1000;
const int HEIGHT = 1000;

const int CAR_DOMAIN_WIDTH = 1200;
const int CAR_DOMAIN_HEIGHT = 1200;

const float CAR_SPEED = 130;
const float HONKING_DISTANCE = 230;
const float SWEAR_DISTANCE = 200;
const float CAR_SPAWN_DELAY = 0.4;

const float DOPPLER_FACTOR = 0.1;

const float AUDIO_SOURCE_ANIMATION_SPEED = 0.25;
const float COLLISION_SOUND_INTERVAL = 0.5;

const int NUM_SWEARS = 30;

const std::string SWEARS[NUM_SWEARS] {
    "../audio/swears/australian1.ogg",
    "../audio/swears/brittish1.ogg",
    "../audio/swears/danish.ogg",
    "../audio/swears/french1.ogg",
    "../audio/swears/french2.ogg",
    "../audio/swears/french3.ogg",
    "../audio/swears/german1.ogg",
    "../audio/swears/german2.ogg",
    "../audio/swears/norwegian1.ogg",
    "../audio/swears/russian1.ogg",
    "../audio/swears/russian2.ogg",
    "../audio/swears/skane1.ogg",
    "../audio/swears/spanish1.ogg",
    "../audio/swears/spanish2.ogg",
    "../audio/swears/spanish3.ogg",
    "../audio/swears/australian1.ogg",
    "../audio/swears/brittish1.ogg",
    "../audio/swears/danish.ogg",
    "../audio/swears/french1.ogg",
    "../audio/swears/french2.ogg",
    "../audio/swears/french3.ogg",
    "../audio/swears/german1.ogg",
    "../audio/swears/german2.ogg",
    "../audio/swears/norwegian1.ogg",
    "../audio/swears/russian1.ogg",
    "../audio/swears/russian2.ogg",
    "../audio/swears/skane1.ogg",
    "../audio/swears/spanish1.ogg",
    "../audio/swears/spanish2.ogg",
    "../audio/swears/spanish3.ogg"
};

struct AudioSource {
    sf::Vector2<float> pos;
    std::vector<cAudio::IAudioSource*> audio;
    float attenuation;

    std::vector<sf::Texture*> textures;
    std::vector<sf::Sprite> sprites;

    std::size_t current_sprite;
    float last_switch;

    void draw(sf::RenderTarget* target);
    void update(float dt);

    float play_rate;
};

enum RoadOrientation {VERTICAL, HORIZONTAL};

struct CarRoad {

    RoadOrientation orientation;

    float pos;

    int dir;

    int num_cars;
};


class Level {

public:

    Level();
    ~Level();

    sf::Vector2<float> get_player_pos() const;
    sf::Vector2<float> get_player_velocity() const;

    void update(float dt);

    void draw(sf::RenderTarget* target);

    //pass true to restart next level. pass false to restart current level
    void change(bool go_to_next);

private:

    bool in_dev_mode;

    std::vector<AudioSource> audio_sources;

    std::vector<cAudio::IAudioSource*> wall_collision_sources;
    std::vector<cAudio::IAudioSource*> wall_collision_voices;
    std::vector<cAudio::IAudioSource*> night_clubs;

    sf::Vector2<float> player_pos;
    sf::Vector2<float> player_velocity;
    sf::Vector2<float> goal_position;

    float player_speed;

    float step_delay;
    float step_timer;

    float time_since_collision_sound = 0;

    Ground* ground;

    sf::Image sound_map;
    sf::Texture level_texture;
    sf::Sprite level_sprite;
    sf::Texture goal_texture;
    sf::Sprite goal_sprite;
    sf::Texture pretty_texture;
    sf::Sprite pretty_sprite;

    cAudio::IAudioManager* audio_manager;
    cAudio::IListener* listener;
#if CAUDIO_EFX_ENABLED == 1
    cAudio::IFilter* lp_filter;
#endif

    int level_num {0};
    std::string map_path;
    bool changed_level {false};
    
    std::vector<Car*> available_cars;
    std::vector<Car*> cars_in_use;
    std::vector<CarRoad> roads;
    float car_timer;

    void maybe_spawn_car(float dt);

    void play_audio_sources();

    /*
     * Reads keyboard input and updates the
     * player velocities.
     */
    void handle_input();

    /*
     * Sets the player velocity such that it doesn't
     * walk through a wall. For example, walking straight
     * into a wall would just set the velocity to (0, 0).
     */
    void handle_collisions(float dt);

    /*
     * Returns true if the player is within the radius of the
     * goal.
     */
    bool has_reached_goal();

#if CAUDIO_EFX_ENABLED == 1
    void handle_night_club_fx();
#endif

    /*
     * Adds the player velocity scaled by the speed
     */
    void update_player_position(float dt);


    void load_json_data();

    /*
     * Draws the current position of the player for debugging
     */
    void debug_draw_player(sf::RenderTarget* target);

    void debug_draw_audio_sources(sf::RenderTarget* target);

    void debug_draw_cars(sf::RenderTarget* target);
    
    Mat::Material ground_under_player();

    /*
     * Decreases step_timer and plays a random step sound
     * when it reaches zero
     */
    void handle_steps(float dt);

    void load_collision_audio();

    void play_collision_sound();

    void update_cars(float dt);

    void load_swears_and_cars();
    
    //displays game over screen offers to restart level or go to next level
    void splash_you_died();

};

#endif /* ifndef LEVEL_H */

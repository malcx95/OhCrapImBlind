#ifndef LEVEL_H
#define LEVEL_H

#include "ground.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cAudio/cAudio.h>
#include "car.hpp"

const float DEFAULT_PLAYER_X = 0.0;
const float DEFAULT_PLAYER_Y = 0.0;

const sf::Vector2<float> UP     = sf::Vector2<float>(0, -1.0);
const sf::Vector2<float> DOWN   = sf::Vector2<float>(0, 1.0);
const sf::Vector2<float> RIGHT  = sf::Vector2<float>(1.0, 0);
const sf::Vector2<float> LEFT   = sf::Vector2<float>(-1.0, 0);
const sf::Vector2<float> STILL  = sf::Vector2<float>(0, 0);

const int GOAL_RADIUS           = 50;

const std::string CAR_ENGINE = "";
const std::string CAR_HONK = "";

const std::string DEFAULT_MAP = "../maps/map-default.png";
const std::string DEFAULT_AUDIO_MAP = "../data/test_audio.json";
const std::string GOAL_SPRITE = "../assets/goal.png";

const sf::Color WOOD = sf::Color(0x60,0x32,0x00, 255);
const sf::Color GRAVEL = sf::Color(0x4b, 0x4b, 0x4b, 255);
const sf::Color GRASS = sf::Color(0x00, 0xff, 0x00, 255);
const sf::Color PUDDLE = sf::Color(0x00, 0x00, 0xff, 255);

const int WIDTH = 1000;
const int HEIGHT = 1000;

// lower number = more cars
const int CAR_SPAWN_RATE = 100;
const float CAR_SPEED = 50;
const float HONKING_DISTANCE = 200;

struct AudioSource {
    sf::Vector2<float> pos;
    cAudio::IAudioSource* audio;
    float attenuation;

    std::vector<std::pair<sf::Texture, sf::Sprite>> sprites;
};

enum RoadDirection {VERTICAL, HORIZONTAL};

struct CarRoad {
    
    RoadDirection direction;

    float pos;

};

class Level {

public:

    Level();
    ~Level();

    sf::Vector2<float> get_player_pos() const;
    sf::Vector2<float> get_player_velocity() const;

    void update(float dt);

    void draw(sf::RenderTarget* target);
    
    void change();
    
private:

    bool in_dev_mode;

    std::vector<AudioSource> audio_sources;

    std::vector<cAudio::IAudioSource*> wall_collision_sources;


    sf::Vector2<float> player_pos;
    sf::Vector2<float> player_velocity;
    sf::Vector2<float> goal_position;

    float player_speed;
    
    float step_delay;
    float step_timer;

    Ground* ground;

    sf::Image sound_map;
    sf::Texture level_texture;
    sf::Sprite level_sprite;
    sf::Texture goal_texture;
    sf::Sprite goal_sprite;

    cAudio::IAudioManager* audio_manager;
    cAudio::IListener* listener;
    
    int level_num {0};
    std::string map_path;
    bool changed_level {false};
    
    

    cAudio::IAudioSource* car_engine;
    cAudio::IAudioSource* car_honk;

    Car* current_car;

    std::vector<CarRoad> roads;

    void maybe_spawn_car();

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
    
    
    Mat::Material ground_under_player();

    /*
     * Decreases step_timer and plays a random step sound
     * when it reaches zero
     */
    void handle_steps(float dt);
    

    void load_collision_audio();

    void play_collision_sound();

    void update_car();

};

#endif /* ifndef LEVEL_H */

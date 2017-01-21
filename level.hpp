#ifndef LEVEL_H
#define LEVEL

#include "ground.hpp"

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cAudio/cAudio.h>

const float DEFAULT_PLAYER_X = 0.0;
const float DEFAULT_PLAYER_Y = 0.0;

const sf::Vector2<float> UP     = sf::Vector2<float>(0, -1.0);
const sf::Vector2<float> DOWN   = sf::Vector2<float>(0, 1.0);
const sf::Vector2<float> RIGHT  = sf::Vector2<float>(1.0, 0);
const sf::Vector2<float> LEFT   = sf::Vector2<float>(-1.0, 0);
const sf::Vector2<float> STILL  = sf::Vector2<float>(0, 0);

const int GOAL_RADIUS           = 50;

const std::string DEFAULT_MAP = "../maps/map-default.png";
const std::string DEFAULT_AUDIO_MAP = "../data/test_audio.json";
const std::string GOAL_SPRITE = "../assets/goal.png";

const sf::Color WOOD = sf::Color(0x60,0x32,0x00, 255);
const sf::Color GRAVEL = sf::Color(0x4b, 0x4b, 0x4b, 255);
const sf::Color GRASS = sf::Color(0x00, 0xff, 0x00, 255);
const sf::Color PUDDLE = sf::Color(0x00, 0x00, 0xff, 255);

const int WIDTH = 1000;
const int HEIGHT = 1000;

struct AudioSource {

    sf::Vector2<float> pos;

    cAudio::IAudioSource* audio;

};

class Level {

public:

    Level();
    ~Level();

    sf::Vector2<float> get_player_pos() const;
    sf::Vector2<float> get_player_velocity() const;

    void update(float dt);

    void draw(sf::RenderTarget* target);

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
    void handle_collisions();

  /*
   * Returns true if the player is within the radius of the
   * goal.
   */
    bool has_reached_goal();

    /*
     * Adds the player velocity scaled by the speed
     */
    void update_player_position();

    Mat::Material ground_under_player();

    /*
     * Decreases step_timer and plays a random step sound
     * when it reaches zero
     */
    void handle_steps(float dt);

    void load_json_data();

    /*
     * Draws the current position of the player for debugging
     */
    void debug_draw_player(sf::RenderTarget* target);

    void debug_draw_audio_sources(sf::RenderTarget* target);

    void load_collision_audio();

    void play_collision_sound();
};

#endif /* ifndef LEVEL_H */

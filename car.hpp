#ifndef CAR_H
#define CAR_H 

#include <SFML/Graphics.hpp>
#include <cAudio/cAudio.h>

const float CAR_WIDTH = 25;

const float HONK_STRENGTH = 100.0;
const float SWEAR_STRENGTH = 100.0;
const float ENGINE_NOISE_STRENGTH = 50.0;

class Car {

public:

    /*
     * Initializes a car with starting position pos, and velocity velocity.
     * The car will travel as fast as the magnitude of velocity and in
     * that direction.
     */
    Car(cAudio::IAudioSource* source, 
            cAudio::IAudioSource* honk_audio_source, 
            cAudio::IAudioSource* swear_audio_source);

    /*
     * Moves the car one step in the direction of it's velocity.
     */
    void update_position(float dt);

    /*
     * Starts the engine noise
     */
    void start(sf::Vector2<float> pos, sf::Vector2<float> velocity);

    /*
     * Stops the engine noise.
     */
    void stop();

    sf::Vector2<float> get_position() const;

    void honk_if_close_to(sf::Vector2<float> pos, float distance);

    void swear_if_close_to(sf::Vector2<float> pos, float distance);

    int get_road_index() const;

    void set_road_index(int index);

    /*
     * Checks whether this car is colliding with something at position
     * pos with width width.
     */
    bool collides_with(sf::Vector2<float> pos) const;

    bool out_of_bounds(int width, int height) const;

private:

    sf::Vector2<float> pos;
    sf::Vector2<float> velocity;
    cAudio::IAudioSource* audio_source;
    cAudio::IAudioSource* honk_audio_source;
    cAudio::IAudioSource* swear_audio_source;
    int road_index;

    float honk_distance;

    bool swore;

    void play_if_close_to(sf::Vector2<float> pos,
            float distance, cAudio::IAudioSource* sound, float strength);

};

#endif /* ifndef CAR_H */

#ifndef CAR_H
#define CAR_H 

#include <SFML/Graphics.hpp>
#include <cAudio/cAudio.h>

const std::string CAR_DRIVE_AUDIO = "";

class Car {

public:

    /*
     * Initializes a car with starting position pos, and velocity velocity.
     * The car will travel as fast as the magnitude of velocity and in
     * that direction.
     */
    Car(sf::Vector2<float> pos, sf::Vector2<float> velocity,
            cAudio::IAudioSource* source,
            cAudio::IAudioSource* honk_audio_source);

    /*
     * Moves the car one step in the direction of it's velocity.
     */
    void update_position();

    sf::Vector2<float> get_position() const;

    void honk_if_close_to(sf::Vector2<float> pos, float distance) const;


    /*
     * Checks whether this car is colliding with something at position
     * pos with width width.
     */
    bool collides_with(sf::Vector2<float> pos, float width) const;

private:

    sf::Vector2<float> pos;
    sf::Vector2<float> velocity;
    cAudio::IAudioSource* audio_source;
    cAudio::IAudioSource* honk_audio_source;

    float honk_distance;

};

#endif /* ifndef CAR_H */

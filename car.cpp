#include "car.hpp"
#include "util.hpp"
#include <cmath>

Car::Car(cAudio::IAudioSource* audio_source,
        cAudio::IAudioSource* honk_audio_source,
        cAudio::IAudioSource* swear_audio_source) {
    this->audio_source = audio_source;
    this->honk_audio_source = honk_audio_source;
    this->swear_audio_source = swear_audio_source;
}

void Car::update_position(float dt) {
    this->pos += this->velocity * dt;
    this->audio_source->move(util::sf_to_caudio_vect(this->pos));
    this->audio_source->setVelocity(
            util::sf_to_caudio_vect(this->velocity));
    this->honk_audio_source->move(util::sf_to_caudio_vect(this->pos));
    this->honk_audio_source->setVelocity(
            util::sf_to_caudio_vect(this->velocity));
    this->swear_audio_source->move(util::sf_to_caudio_vect(this->pos));
    this->swear_audio_source->setVelocity(
            util::sf_to_caudio_vect(this->velocity));
}

sf::Vector2<float> Car::get_position() const {
    return this->pos;
}

void Car::honk_if_close_to(sf::Vector2<float> pos, float distance) {
    play_if_close_to(pos, distance, this->honk_audio_source, HONK_STRENGTH);
}

void Car::swear_if_close_to(sf::Vector2<float> pos, float distance) {
    play_if_close_to(pos, distance, this->swear_audio_source,
            SWEAR_STRENGTH);
}

void Car::play_if_close_to(sf::Vector2<float> pos,
        float distance, cAudio::IAudioSource* sound, float strength) {
    float dist = util::distance(pos, this->pos);
    
    // we assume the car only goes straight in the x or y direction
    float p;
    if (this->velocity.x == 0) {
        p = fabs(this->pos.x - pos.x);
    } else {
        p = fabs(this->pos.y - pos.y);
    }
    if (p < CAR_WIDTH && dist < distance && !sound->isPlaying()) {
        sound->play3d(util::sf_to_caudio_vect(this->pos), strength, false);
    }
}

bool Car::collides_with(sf::Vector2<float> pos) const {
    sf::Vector2<float> p_pos = pos - this->pos;
    return sqrt(p_pos.x * p_pos.x + p_pos.y * p_pos.y) < CAR_WIDTH;
}

void Car::start(sf::Vector2<float> pos, sf::Vector2<float> velocity) {
    this->pos = pos;
    this->velocity = velocity;
    this->audio_source->play3d(
            util::sf_to_caudio_vect(this->pos), ENGINE_NOISE_STRENGTH, true);
}

void Car::stop() {
    this->audio_source->stop();
}

bool Car::out_of_bounds(int width, int height) const {
    return pos.x < -width || pos.y < -width || 
        pos.x > width || pos.y > height;
}


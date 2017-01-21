#include "car.hpp"
#include "util.hpp"
#include <cmath>

Car::Car(sf::Vector2<float> pos, sf::Vector2<float> velocity,
        cAudio::IAudioSource* audio_source,
        cAudio::IAudioSource* honk_audio_source) {
    this->pos = pos;
    this->velocity = velocity;
    this->audio_source = audio_source;
    this->honk_audio_source = honk_audio_source;
}

void Car::update_position() {
    this->pos += this->velocity;
    this->audio_source->move(util::sf_to_caudio_vect(this->pos));
    this->honk_audio_source->move(util::sf_to_caudio_vect(this->pos));
}

sf::Vector2<float> Car::get_position() const {
    return this->pos;
}

void Car::honk_if_close_to(sf::Vector2<float> pos, float distance) const {
    sf::Vector2<float> p_pos = pos - this->pos;
    float phi = fabs(atan2(p_pos.y, p_pos.x) - 
            atan2(this->pos.y, this->pos.x));
    float dis = sqrt(p_pos.x * p_pos.x + p_pos.y * p_pos.y);
    float gamma = fabs(atan2(dis, CAR_WIDTH));

    if (phi > gamma && dis < distance && 
            !this->honk_audio_source->isPlaying()) {
        this->honk_audio_source->play3d(
                util::sf_to_caudio_vect(this->pos), HONK_STRENGTH, false);
    }
}

bool Car::collides_with(sf::Vector2<float> pos, float width) const {
    sf::Vector2<float> p_pos = pos - this->pos;
    return sqrt(p_pos.x * p_pos.x + p_pos.y * p_pos.y) < width;
}

void Car::start() {
    this->audio_source->play3d(
            util::sf_to_caudio_vect(this->pos), ENGINE_NOISE_STRENGTH, true);
}

void Car::stop() {
    this->audio_source->stop();
}

bool Car::out_of_bounds(int width, int height) const {
    return pos.x < 0 || pos.y < 0 || pos.x > width || pos.y > height;
}


#include "util.hpp"

cAudio::cVector3 util::sf_to_caudio_vect(sf::Vector2<float>& vect) {
    return cAudio::cVector3(vect.x, 0, vect.y);
}


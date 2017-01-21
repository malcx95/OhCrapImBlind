#ifndef UTIL_H
#define UTIL_H 

#include <cAudio/cVector3.h>
#include <SFML/Graphics.hpp>

namespace util {

    cAudio::cVector3 sf_to_caudio_vect(sf::Vector2<float>& vect);

    // calculates the distance between two points
    float distance(sf::Vector2<float> point1, sf::Vector2<float> point2);

}

#endif /* ifndef UTIL_H */

#ifndef UTIL_H
#define UTIL_H 

#include <cAudio/cVector3.h>
#include <SFML/Graphics.hpp>

namespace util {

    cAudio::cVector3 sf_to_caudio_vect(sf::Vector2<float>& vect);

}

#endif /* ifndef UTIL_H */

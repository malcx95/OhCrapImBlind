#include "util.hpp"
#include <math.h>

cAudio::cVector3 util::sf_to_caudio_vect(sf::Vector2<float> vect) {
    return cAudio::cVector3(vect.x, 0, vect.y);
}

float util::distance(sf::Vector2<float> point1, sf::Vector2<float> point2) {
  return sqrt((pow((point1.x-point2.x),2))+pow((point1.y-point2.y),2));
}

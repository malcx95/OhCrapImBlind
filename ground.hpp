#ifndef GROUND_HPP
#define GROUND_HPP

#include <vector>

namespace cAudio {
    class IAudioManager;
    class IAudioSource;
}

namespace Mat {
    enum Material {
        WOOD = 0,
        GRAVEL,
        GRASS,
        PUDDLE
    };
}

class Ground {
public:
    explicit Ground(cAudio::IAudioManager* audio_manager);
    void play_random_step(Mat::Material material);

private:
    int last_index;
    std::vector<std::vector<cAudio::IAudioSource*>> step_sounds;
};

#endif

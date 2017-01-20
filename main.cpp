#include <iostream>
#include <cAudio/cAudio.h>
#include <cstdlib>
#include <cmath>
#include <ctime>

void init() {
    srand(time(NULL));
}

int generate_random_index(int last_index, unsigned int length) {
    // A negative last_index means that you don't want to skip any index
    if (last_index > 0) {
        int random_n = rand() % length;
        if (random_n >= last_index) {
            return random_n + 1;
        } else {
            return random_n;
        }
    }
    return rand() % (length - 1);
}

void test_audio() {
    cAudio::IAudioManager* audio_mgr = cAudio::createAudioManager(true);
    if (!audio_mgr) {
        std::cerr << "ERROR: Could not create audio manager" << std::endl;
        exit(EXIT_FAILURE);
    }

    cAudio::IListener* listener = audio_mgr->getListener();
    listener->setPosition(cAudio::cVector3(0, 0, 0));

    if (!listener) {
        std::cerr << "ERROR: Could not create listener" << std::endl;
        exit(EXIT_FAILURE);
    }

    cAudio::IAudioSource* my_sound = audio_mgr->create("roger", "Roger.ogg", true);
    if (!my_sound) {
        std::cerr << "ERROR: Could not load Roger.ogg" << std::endl;
        exit(EXIT_FAILURE);
    }

    my_sound->play3d(cAudio::cVector3(0, 0, 0), 2.0f, true);
    my_sound->setVolume(1.0f);
    my_sound->setMinDistance(1.0f);
    my_sound->setMaxAttenuationDistance(100.0f);

    const int ticksToPlay = 10000;
    int currentTick = 0;
    int currentSecTick = 0;
    float rot = 0.0f;

    while (my_sound->isPlaying() && currentTick < ticksToPlay) {
        rot += 0.1f * 0.017453293f;

        float x = 5.0f * cosf(rot);
        float z = 5.0f * sinf(rot);
        my_sound->move(cAudio::cVector3(x, 0.0f, z));

        currentTick++;

        if (currentTick / 1000 > currentSecTick) {
            currentSecTick++;
            std::cout << ".";
        }

        cAudio::cAudioSleep(1);
    }
    std::cout << std::endl;

    cAudio::destroyAudioManager(audio_mgr);
}

int main() {
    return 0;
}

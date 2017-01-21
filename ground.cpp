#include "ground.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>

#include <cAudio/cAudio.h>

int generate_random_index(int last_index, unsigned int length) {
    // A negative last_index means that you don't want to skip any index
    if (last_index > 0) {
        int random_n = rand() % (length - 1);
        if (random_n >= last_index) {
            return random_n + 1;
        } else {
            return random_n;
        }
    }
    return rand() % length;
}

Ground::Ground(cAudio::IAudioManager* audio_manager) {
    const std::vector<Mat::Material> materials {
        Mat::WOOD, Mat::GRAVEL, Mat::GRASS, Mat::PUDDLE
    };
    step_sounds.resize(materials.size());

    const std::vector<std::vector<std::string>> step_files {
        {
            "wood1.ogg",
            "wood2.ogg",
            "wood3.ogg",
            "wood4.ogg",
            "wood5.ogg",
            "wood6.ogg"
        },
        {
            "gravel1.ogg",
            "gravel2.ogg",
            "gravel3.ogg",
            "gravel4.ogg",
            "gravel5.ogg",
            "gravel6.ogg",
            
        },
        {
            "grass1.ogg",
            "grass2.ogg",
            "grass3.ogg",
            "grass4.ogg",
            "grass5.ogg",
        },
        {
            "water1.ogg",
            "water2.ogg",
            "water3.ogg",
            "water4.ogg",
            "water5.ogg",
        }
    };

    const std::vector<std::string> material_folders {
        "../audio/wood/",
        "../audio/gravel/",
        "../audio/grass/",
        "../audio/water/",
    };

    for (auto material : materials) {
        for (auto filename : step_files[material]) {
            std::string full_name = material_folders[material] + filename;
            cAudio::IAudioSource* sound = audio_manager->create(
                filename.c_str(), full_name.c_str(), true
            );

            if (!sound) {
                std::cerr << "ERROR: Could not load " << full_name << std::endl;
                exit(EXIT_FAILURE);
            }
            step_sounds[material].push_back(sound);
        }
    }

}

void Ground::play_random_step(Mat::Material material) {
    int available_sounds = step_sounds[material].size();
    if (available_sounds > 0) {
        int random_n = generate_random_index(last_index, available_sounds);
        step_sounds[material][random_n]->play2d(false);
        last_index = random_n;
    }
}

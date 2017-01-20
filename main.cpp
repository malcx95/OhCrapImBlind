#include "level.hpp"
#include <unistd.h>

int main() {
    Level* l = new Level();
    while (true) {
        l->update();
        usleep(100000);
    }
    return 0;
}

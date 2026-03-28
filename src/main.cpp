
#include <iostream>
#include "../include/backends/linux/alsabackend.h"

    int main(int argc,char *argv[]) {

        AudioEngine::ALSABackend backend;
        backend.iniitialize();
        backend.play_sine(440.0f);
        return 0;
    }




#include <iostream>
#include "../include/backends/linux/alsabackend.h"

    int main(int argc,char *argv[]) {

        AudioEngine::ALSABackend backend;
        std::vector<AudioEngine::AudioDevice> devices = backend.enumerate_devices();
        for(const auto d : devices) {
                std::cout << d << std::endl;
        }
        return 0;
    }



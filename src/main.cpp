
#include <iostream>
#include "../include/backends/linux/alsabackend.h"

    int main(int argc,char *argv[]) {

        AudioEngine::ALSABackend backend;
        backend.iniitialize();
        backend.open_stream();
        backend.start_stream();
        int i =0;
        while (true)
        {
            i+= 5;
            std::cout << i << std::endl;
            sleep(5);

        }
        
        //backend.play_sine(440.0f);
        return 0;
    }



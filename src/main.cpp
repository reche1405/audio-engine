
#include <iostream>
//#include "../include/backends/linux/alsabackend.h"
#include "../include/audioengine.h"
#include "../include/testprocess.h"
int main(int argc,char *argv[]) {

        ioengine::AudioEngine engine;
        ioengine::TestProcess process;
        engine.set_process(&process);
        engine.initialise();
        int i =0;
        std::cout << "Playback devices" << std::endl;
        for (auto dev : engine.list_playback_devices()) {

            std::cout << dev << std::endl;
        }
       /*  std::cout << std::endl;
       while (true)
       {
            sleep(120);
            i+= 2;
            std::cout << i << std::endl;

        }  */
        return 0;
    }



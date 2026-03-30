
#include <iostream>
//#include "../include/backends/linux/alsabackend.h"
#include "../include/audioengine.h"
#include "../include/testprocess.h"
int main(int argc,char *argv[]) {

        AudioEngine::AudioEngine engine;
        AudioEngine::TestProcess process;
        engine.set_process(&process);
        engine.initialise();
        int i =0;
       while (true)
        {
            i+= 5;
            std::cout << i << std::endl;
            sleep(5);

        } 
        return 0;
    }



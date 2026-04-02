
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
       /*  std::cout << std::endl;
       while (true)
       {
            sleep(120);
            i+= 2;
            std::cout << i << std::endl;

        }  */
        return 0;
    }



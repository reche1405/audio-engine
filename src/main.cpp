
#include <iostream>
//#include "../include/backends/linux/alsabackend.h"
#include "../include/audioengine.h"
#include "../include/testprocess.h"
#include <chrono>
#include <thread>
int main(int argc,char *argv[]) {

    ioengine::AudioEngine engine;
    ioengine::TestProcess process;
    std::cout << "This is a new test!!!!" << std::endl;

    engine.set_process(&process);
    engine.initialise();
    int i =0;
    std::cout << "Playback deviceroonies" << std::endl;
    std::vector<ioengine::AudioDevice> devs = engine.list_playback_devices();
   /*  for (auto dev : devs) {
        
        std::cout << dev << std::endl << "Extra Info ...... " << std::endl;
    } */
    std::cout << "Default 0 samples" << std::endl;
    int nSamples = 0;
    int blockSizeFrames = 512;
    std::vector<float> buffer; 
    int blockSizeSamples = blockSizeFrames * 2;
    buffer.resize(blockSizeSamples);
    // Pre fill the ring buffer with audio data from the processor (Tesrt Sine Wave)..
    
    while(nSamples < 2048) {
        process.process_audio(buffer.data(), blockSizeFrames);
        engine.ring_buffer().pushBlock(buffer.data(), blockSizeSamples);
        nSamples += blockSizeSamples;
    }
    engine.open_stream();
    engine.start_stream();
    std::cout << "Stream has begun" << std::endl;
    while (true)
    {

        nSamples = engine.ring_buffer().availableSamples();
        if(nSamples < 2048 - blockSizeSamples) {
            while(nSamples < 2048 - blockSizeSamples) {
                
                process.process_audio(buffer.data(), blockSizeFrames);
                engine.ring_buffer().pushBlock(buffer.data(),blockSizeFrames * 2);
                nSamples += blockSizeSamples;
            }
        };
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

    };
        return 0;
    }



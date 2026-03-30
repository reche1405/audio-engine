#ifndef TESTPROCEES_H
#define TESTPROCESS_H
#include "./common/audioprocess.h"
#include <atomic> 
#include <cmath>
namespace AudioEngine {
    class TestProcess : public AudioProcess {
        public:
            TestProcess() : AudioProcess() {};
            ~TestProcess() = default;
            void process_audio(float *buffer, uint32_t frames) override;
        private:
            std::atomic<float> phase = 0.0f; 
            std::atomic<float> frequency = 440.0f; // A4 note
            float sampleRate = 48000.0f;;
            float amplitude = 0.5f; 
    };
}
#endif
#ifndef TESTPROCEES_H
#define TESTPROCESS_H
#define _USE_MATH_DEFINES
#include "./common/audioprocess.h"
#include <atomic> 
#include <cmath>
#include <iostream>

namespace ioengine {
    class TestProcess : public AudioProcess {
        public:
            TestProcess() : AudioProcess() {};
            ~TestProcess() = default;
            void process_audio(float *buffer, uint32_t frames) override;
        private:
            float sampleRate = 48000.0f;
            const float phaseIncrement = 2.0f * 3.14159265358979323846f * 440.0f / sampleRate;
            const float phaseBoundary = 2.0f * 3.14159265358979323846f;
            std::atomic<float> phase = 0.0f; 
            std::atomic<float> frequency = 440.0f; // A4 note
            float amplitude = 0.5f; 
    };
}
#endif
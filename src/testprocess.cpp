#define _USE_MATH_DEFINES
#include "../include/testprocess.h"
#include <cmath>
#include <iostream>
namespace ioengine {
    void TestProcess::process_audio(float *buffer, uint32_t frames)  {
        
        float localPhase = phase.load();
        for (int i = 0; i < frames; i++ ) {
            float sample = 0.5f * sinf(localPhase);
            buffer[i * 2] = sample;
            buffer[(i * 2) + 1] = sample;

            localPhase += phaseIncrement;

            // Keep phase within [0, 2*PI] to avoid precision drift
            if (localPhase >= phaseBoundary) {
                localPhase -= phaseBoundary;
            }
        }
        phase.store(localPhase);
    }
}
#define _USE_MATH_DEFINES
#include "../include/testprocess.h"
#include <cmath>

namespace AudioEngine {
    void TestProcess::process_audio(float *buffer, uint32_t frames)  {
        float localPhase = phase.load();
        for (int i = 0; i < frames * 2; i++) {
            buffer[i] = 0.5f * sinf(localPhase);

            localPhase += 2.0f * M_PI * 440.0f / sampleRate;

            // Keep phase within [0, 2*PI] to avoid precision drift
            if (localPhase >= 2.0f * M_PI) {
                localPhase -= 2.0f * M_PI;
            }
        }
        phase.store(localPhase);
    }
}
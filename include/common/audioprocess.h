#ifndef AUDIOPROCESS_H
#define UADIOPROCESS_H
#include <cstdint>
namespace AudioEngine{
    class AudioProcess {
        public:
            ~AudioProcess() = default;

            virtual void process_audio(float* buffer, uint32_t frames);

    };
}
#endif
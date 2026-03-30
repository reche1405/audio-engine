#ifndef AUDIOPROCESS_H
#define AUDIOPROCESS_H
#include <cstdint>
namespace AudioEngine{
    class AudioProcess {
        public:
            ~AudioProcess() = default;

            virtual void process_audio(float* buffer, uint32_t frames) = 0; 
        protected:
            uint32_t m_sampleRate;

    };
}
#endif
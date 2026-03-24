#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H
#include "audioconfig.h"
namespace AudioEngine {

    class AudioBuffer {
        public:

            AudioBuffer(BufferFormat format = BufferFormat::Planar) {
                bufferFormat = format;
            }

            uint32_t num_channels() const {
                return numChannels;
            };
            uint32_t num_frames() const {
                return numFrames;
            };
            float** data() const {
                return channels;
            };
            void convert_format(BufferFormat desiredFormat);
        private:
            float** channels;
            uint32_t numChannels;
            uint32_t numFrames; 
            BufferFormat bufferFormat;
    };
}

#endif
#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H
#include <vector>
#include "audioconfig.h"
namespace ioengine {
    template <typename T> 
    class AudioBuffer {
        public:

            AudioBuffer(uint32_t nChannels, uint32_t nFrames, BufferFormat format = BufferFormat::Interleaved) {
                numChannels = nChannels;
                numFrames = nFrames;
                bufferFormat = format;
            }

            uint32_t num_channels() const {
                return numChannels;
            };
            uint32_t num_frames() const {
                return numFrames;
            };
            T* data()  {
                return channels.data();
            };
            void convert_format(BufferFormat desiredFormat);
        private:
            std::vector<T> channels;
            uint32_t numChannels;
            uint32_t numFrames; 
            BufferFormat bufferFormat;
    };
    using AudioBufferF = AudioBuffer<float>;
}

#endif
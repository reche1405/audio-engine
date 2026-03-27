#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H
#include "../common/audioconfig.h"
#include <string>
#include <algorithm>
namespace AudioEngine {
    struct AudioDevice {
        public: 
            std::string name; 
            std::string UID;
            DeviceCapabilities capabilities; 

    };
    std::ostream& operator<<(std::ostream& os, const AudioDevice& dev) {
    os << "--- Audio Device ---\n"
       << "  Name:     " << dev.name << "\n"
       << "  ID:       " << dev.UID << "\n"
       << "  Channels: Out[" << dev.capabilities.maxOutputChannels << "] In[" << dev.capabilities.maxInputChannels << "]\n";
    os << "  Sample Rates:    " << "\n";
    for (unsigned int rate : dev.capabilities.supportedSampleRates) {
    os << rate << " Hz\n";   
    }

    os << "  Buffer Sizes:    " << "\n";
    for (unsigned int size : dev.capabilities.supportedBufferSizes) {
    os << size << " samples\n";   
    }


    BufferFormat planar = BufferFormat::Planar;
    BufferFormat inter = BufferFormat::Interleaved;
    os << "  Buffer Formats:    " << "\n";
    if(std::find(dev.capabilities.supportedBufferFormats.begin(),
                 dev.capabilities.supportedBufferFormats.end(),planar ) != 
                 dev.capabilities.supportedBufferFormats.end()) {
    os << "Planar Support\n";
    }
     if(std::find(dev.capabilities.supportedBufferFormats.begin(),
                 dev.capabilities.supportedBufferFormats.end(),inter ) != 
                 dev.capabilities.supportedBufferFormats.end()) {
    os << "Interleaved Support\n";
    }
    return os;
}
}

#endif
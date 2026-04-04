#include "../../include/devices/audiodevice.h"
namespace ioengine {

    std::ostream& operator<<(std::ostream& os, const AudioDevice& dev) {
       os << "--- Audio Device ---\n"
          << "  Name:     " << dev.name << "\n"
          << "  ID:       " << dev.UID << "\n"
          << "  Channels: Out[" << dev.capabilities.maxOutputChannels << "] In[" << dev.capabilities.maxInputChannels << "]\n";
       os << "  Sample Rates:    " << "\n";
       for (unsigned int rate : dev.capabilities.supportedSampleRates) {
       os << "  -   " << rate << " Hz\n";   
       }
   
       os << "  Buffer Sizes:    " << "\n";
       for (uint64_t size : dev.capabilities.supportedBufferSizes) {
       os << "  -   " << size << " \n";   
       }
   
   
       BufferFormat planar = BufferFormat::Planar;
       BufferFormat inter = BufferFormat::Interleaved;
       os << "  Buffer Formats:    " << "\n";
       if(std::find(dev.capabilities.supportedBufferFormats.begin(),
                    dev.capabilities.supportedBufferFormats.end(),planar ) != 
                    dev.capabilities.supportedBufferFormats.end()) {
       os << "  -   Planar \n";
       }
        if(std::find(dev.capabilities.supportedBufferFormats.begin(),
                    dev.capabilities.supportedBufferFormats.end(),inter ) != 
                    dev.capabilities.supportedBufferFormats.end()) {
       os << "  -   Interleaved \n";
       }
       os << "  Sample Formats:    " << "\n";
       SampleFormat f32 = SampleFormat::Float32;
       SampleFormat i16 = SampleFormat::Int16;
        if(std::find(dev.capabilities.supportedFormats.begin(),
                    dev.capabilities.supportedFormats.end(),f32 ) != 
                    dev.capabilities.supportedFormats.end()) {
       os << "  -   float 32 bit Little Endian \n";
       }
        if(std::find(dev.capabilities.supportedFormats.begin(),
                    dev.capabilities.supportedFormats.end(),i16 ) != 
                    dev.capabilities.supportedFormats.end()) {
       os << "  -   int 16 bit Little Endian  \n";
       }

       return os;
   }
}

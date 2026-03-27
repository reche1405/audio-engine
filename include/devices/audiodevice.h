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
    std::ostream& operator<<(std::ostream& os, const AudioDevice& dev);
}

#endif
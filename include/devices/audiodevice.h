#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H
#include "../common/audioconfig.h"
#include <string>
namespace AudioEngine {
    struct AudioDevice {
        public: 
            std::string name; 
            std::string UID;
            DeviceCapabilities capabilities; 

    };
}

#endif
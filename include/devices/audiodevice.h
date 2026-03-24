#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H
#include "../common/audioconfig.h"
#include <string>
namespace AudioEngine {
    class AudioDevice {
        public:
            AudioDevice(std::string name, std::string UID, DeviceCapabilities capabilities ) {
                m_name = name;
                m_UID = UID; 
                m_capabilities = capabilities;
            }
        private: 
            std::string m_name; 
            std::string m_UID;
            DeviceCapabilities m_capabilities; 

    };
}

#endif
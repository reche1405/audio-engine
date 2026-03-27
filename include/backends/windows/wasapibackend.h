#ifndef WASAPIBACKEND_H
#define WASAPIBACKEND_H
#include "../audiobackend.h"

//#include <AudioClient.h>
namespace AudioEngine {
    class WASAPIBackend : public IAudioBackend
    {
        public:
            WASAPIBackend() : IAudioBackend(BackendType::WASAPI) {

            }

            std::vector<AudioDevice> enumerate_devices() const override;

        
    };
}

#endif
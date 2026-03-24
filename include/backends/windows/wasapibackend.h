#ifndef WASAPIBACKEND_H
#define WASAPIBACKEND_H
#include "../audiobackend.h"
//#include <audioclient.h>
namespace AudioEngine {
    class WASAPIBackend : public IAudioBackend
    {
        public:
            WASAPIBackend() : IAudioBackend(BackendType::WASAPI) {

            }

        
    };
}

#endif
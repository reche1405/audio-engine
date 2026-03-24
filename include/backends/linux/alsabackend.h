#ifndef ALSABACKEND_H
#define ALSABACKEND_H

#include "../audiobackend.h"
namespace AudioEngine {
    class ALSABackend : public IAudioBackend {
        public:
            ALSABackend() : IAudioBackend(BackendType::ALSA) {

            }
    };
}


#endif
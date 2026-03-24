#ifndef ALSABACKEND_H
#define ALSABACKEND_H
#include <vector>
#include <alsa/asoundlib.h>
#include "../audiobackend.h"
namespace AudioEngine {
    class ALSABackend : public IAudioBackend {
        public:
            ALSABackend() : IAudioBackend(BackendType::ALSA) {

            }

            std::vector<AudioDevice> enumerate_devices() override;
    };
}


#endif
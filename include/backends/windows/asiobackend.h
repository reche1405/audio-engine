#ifndef ASIOBACKEND_H
#define ASIOBACKEND_H

#include "../audiobackend.h"
namespace ioengine {
    class ASIOBackend : public IAudioBackend {
        public:
            ASIOBackend() : IAudioBackend(BackendType::ASIO) {

        }
            
    };
}
#endif
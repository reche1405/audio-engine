#ifndef BACKENDFACTORY_H
#define BACKENDFACTORY_H
#include "../common/audioconfig.h"
#include "audiobackend.h"
#include "./linux/alsabackend.h"
#include <memory>
namespace AudioEngine {
    class BackendFactory {
        public:
            ~BackendFactory() = default;
            static std::unique_ptr<IAudioBackend> createBackend(BackendType type = BackendType::Auto) {
                #ifdef __linux__                    
                    return std::make_unique<ALSABackend>();
                #elif _WIN32
                    return std::make_unique<WASAPIBackend>();
                #endif
            }
        private:
            

    };
}
#endif 
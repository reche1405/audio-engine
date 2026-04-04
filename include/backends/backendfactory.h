#ifndef BACKENDFACTORY_H
#define BACKENDFACTORY_H
#include "../common/audioconfig.h"
#include "audiobackend.h"
#include "./linux/alsabackend.h"
#include "./windows/wasapibackend.h"
#include <memory>
namespace ioengine {
    class BackendFactory {
        public:
            ~BackendFactory() = default;
            static std::unique_ptr<IAudioBackend> create_backend(BackendType type = BackendType::Auto) {
                #ifdef __linux__      
                switch(type) {
                    case(BackendType::Auto):
                        return std::make_unique<ALSABackend>();
                        break;
                    default:
                        return std::make_unique<ALSABackend>();
                        break;
                }  
                #endif            
                #ifdef _WIN32
                switch(type) {
                    case (BackendType::Auto):
                        return std::make_unique<WASAPIBackend>();
                        break;
                    default:
                        return std::make_unique<WASAPIBackend>();
                        break;
                }
                #endif
            }
        private:
            

    };
}
#endif 
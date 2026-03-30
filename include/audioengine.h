#ifndef AUDIOENGINE_H 
#define AUDIOENGINE_H
#include "./common/audioprocess.h"
#include "./backends/audiobackend.h"
#include "./backends/backendfactory.h"
#include <memory>
#include <cstdint>
namespace AudioEngine {
    class AudioEngine : public Listener {
        public:
            ~AudioEngine() = default;

            void initialise() {
                m_backend = BackendFactory::createBackend();
                m_backend.get()->set_listener(this);
                m_backend.get()->iniitialize();

            }


            void set_process(AudioProcess* process) {
                m_process = process;
            }

            void on_buffer_request(float* buffer, uint32_t frames) override {
                m_process->process_audio(buffer, frames);
            };
            

        private:
            AudioProcess *m_process = nullptr;
            std::unique_ptr<IAudioBackend> m_backend = nullptr;
    };
}
#endif
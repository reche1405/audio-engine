#ifndef AUDIOENGINE_H 
#define AUDIOENGINE_H
#include "./common/audioprocess.h"
#include "./backends/audiobackend.h"
#include "./backends/backendfactory.h"
#include "./common/ringbuffer.h"
#include "./common/listener.h"
#include <memory>
#include <cstdint>
namespace ioengine {
    class AudioEngine : public Listener {
        public:
            ~AudioEngine() = default;

            void initialise() {
                m_backend = BackendFactory::create_backend();
                m_backend.get()->set_listener(this);
                m_backend.get()->iniitialize();

            };


            void set_process(AudioProcess* process) {
                m_process = process;
            }

            void on_buffer_request(float* buffer, uint32_t frames) override {
                m_process->process_audio(buffer, frames);
            };
            
            RingBuffer<float> &ring_buffer() override {
                return m_ringBuffer;
            }
            std::vector<AudioDevice> list_capture_devices();
            std::vector<AudioDevice> list_playback_devices();
            void open_stream();
            void start_stream();
        private:
            AudioProcess *m_process = nullptr;
            std::unique_ptr<IAudioBackend> m_backend = nullptr;
            RingBuffer<float> m_ringBuffer{4096};
    };
}
#endif
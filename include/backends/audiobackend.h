#ifndef AUDIOBACKEND_H
#define AUDIOBACKEND_H
#include <optional>
#include <vector>
#include <atomic>
#include <memory>
#include "../common/audioconfig.h"
#include "../common/audiobuffer.h"
#include "../devices/audiodevice.h"
namespace AudioEngine {
    class IAudioBackend {
        public:
            IAudioBackend(BackendType type = BackendType::Auto) : m_type(type) {
                
            }
            ~IAudioBackend() = default;
            // List Devices

            virtual void iniitialize() = 0;
            virtual std::vector<AudioDevice> enumerate_devices() = 0;
            void set_listener(Listener *listener) {
                m_listener = listener;
            }

            // Select Devices

            virtual void set_input_device(AudioDevice &dev) = 0;

            virtual void set_output_device(AudioDevice &dev) = 0;


            // Sample Rate Management
            virtual void set_sample_rate(int sampleRate) = 0;
            
            // Buffer Size Management
            virtual void set_buffer_szie(int bufferSize) = 0;

            // Audio Callback
            virtual void process_audio(float* input, float* output, StreamContext& context ) = 0;
            
            // Stream Management
            virtual void open_stream() = 0;
            virtual void close_stream() = 0;
            virtual void run() =0;
            virtual void start_stream() = 0;
            virtual void stop_stream() = 0;




        protected:
            BackendType m_type;
            int m_sampleRate = DEFAULT_SAMPLE_RATE;
            int m_bufferSize = DEFAULT_BUFFER_SIZE;
            std::vector<AudioDevice> m_deviceCache;
            AudioDevice m_playbackDevice;
            AudioDevice m_captureDevice;
            StreamConfig m_config;
            std::atomic<bool> m_running{false};
            Listener * m_listener;
    };
}

#endif 
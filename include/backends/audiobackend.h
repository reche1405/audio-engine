#ifndef AUDIOBACKEND_H
#define AUDIOBACKEND_H
#include <vector>
#include <atomic>
#include <memory>
#include <thread>
#include "../common/audioconfig.h"
#include "../common/listener.h"
#include "../common/audiobuffer.h"
#include "../devices/audiodevice.h"
namespace ioengine {
    class IAudioBackend {
        public:
            IAudioBackend(BackendType type = BackendType::Auto) : m_type(type) {
                
            }
            ~IAudioBackend() = default;
            // List Devices

            virtual void iniitialize() = 0;
            virtual std::vector<AudioDevice> enumerate_devices() = 0;
            std::vector<AudioDevice> list_devices() {
                return m_deviceCache;
            }

            void set_listener(Listener *listener) {
                m_listener = listener;
            }

            // Select Devices

             void set_input_device(AudioDevice &dev) {
        
                m_config.inputDeviceName = dev.UID;
                m_captureDevice = dev;
            };

            void set_output_device(AudioDevice &dev) {
        
                m_config.outputDeviceName = dev.UID;
                m_playbackDevice = dev;
            };


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
            std::thread m_thread;
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
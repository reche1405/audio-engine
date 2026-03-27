#ifndef AUDIOBACKEND_H
#define AUDIOBACKEND_H
#include <optional>
#include <vector>
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
            virtual std::vector<AudioDevice> enumerate_devices() = 0;

            virtual DeviceCapabilities device_capabilities(std::string deviceUID) = 0;
            // Select Devices

            virtual void set_input_device() = 0;

            virtual void set_output_device() = 0;


            // Sample Rate Management
            virtual void set_sample_rate(int sampleRate) = 0;
            
            // Buffer Size Management
            virtual void set_buffer_szie(int bufferSize) = 0;

            // Audio Callback
            virtual void process_audio(AudioBuffer& input, AudioBuffer& output, StreamContext& context ) = 0;
            
            // Stream Management
            virtual void open_stream(StreamConfig config) = 0;
            virtual void close_stream() = 0;

            virtual void start_stream() = 0;
            virtual void stop_stream() = 0;




        protected:
            BackendType m_type;
            int m_sampleRate = DEFAULT_SAMPLE_RATE;
            int m_bufferSize = DEFAULT_BUFFER_SIZE;
    };
}

#endif 
#ifdef _WIN32
#ifndef WASAPIBACKEND_H
#define WASAPIBACKEND_H
#include "../audiobackend.h"

#include <AudioClient.h>
namespace AudioEngine {
    class WASAPIBackend : public IAudioBackend
    {
        public:
            WASAPIBackend() : IAudioBackend(BackendType::WASAPI) {

            }
            void iniitialize() override;
            
            std::vector<AudioDevice> enumerate_devices() override;
            void set_input_device(AudioDevice &dev) override;

            void set_output_device(AudioDevice &dev) override;


            // Sample Rate Management
            void set_sample_rate(int sampleRate) override;
            
            // Buffer Size Management
            void set_buffer_szie(int bufferSize) override;


            // Stream Management
            void open_stream() override;
            bool open_playback();
            bool open_capture();
            void close_stream() override;
            void run() override;
            void start_stream() override;
            void stop_stream() override;
        
    };
}

#endif
#endif
#ifndef ALSABACKEND_H
#define ALSABACKEND_H
#include <vector>
#include <alsa/asoundlib.h>
#include "../audiobackend.h"
#include <string>
#include <iostream>
namespace AudioEngine {
    /* std::vector<std::pair<BufferFormat, snd_pcm_access_t>> ABuffFormats {
        {BufferFormat::Interleaved, SND_PCM_ACCESS_RW_INTERLEAVED},
        {BufferFormat::Planar, SND_PCM_ACCESS_RW_NONINTERLEAVED }
    }; */
    class ALSABackend : public IAudioBackend {
        public:
            ALSABackend() : IAudioBackend(BackendType::ALSA) {

            }

            ~ALSABackend() {
                if(m_handle) {
                    snd_pcm_close(m_handle);
                }
            };
            void iniitialize() override;
            std::vector<AudioDevice> enumerate_devices() override;
            void probe_device_capabilities(AudioDevice& dev) {
                //snd_pcm_t* handle;
                snd_pcm_hw_params_t* params;

                // Open the device for playback to probe it
                if (snd_pcm_open(&m_handle, dev.UID.c_str(), SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK) < 0) {
                    return; // Device busy or unavailable
                }

                snd_pcm_hw_params_alloca(&params);
                snd_pcm_hw_params_any(m_handle, params);

                // Get Supported Sample formats
                if (snd_pcm_hw_params_test_format(m_handle, params, SND_PCM_FORMAT_FLOAT_LE) == 0) {
                    std::cout << "Device supports S32 Float LE!" << std::endl;

                    dev.capabilities.supportedFormats.push_back(SampleFormat::Float32);
                }

                if (snd_pcm_hw_params_test_format(m_handle, params, SND_PCM_FORMAT_S16_LE) == 0) {
                    std::cout << "Device supports S16 Integer!" << std::endl;
                    dev.capabilities.supportedFormats.push_back(SampleFormat::Int16);

                }


                DeviceCapabilities &caps = dev.capabilities;

                // Get Channels
                snd_pcm_hw_params_get_channels_max(params, &caps.maxOutputChannels);

                unsigned int minRate;
                unsigned int maxRate;

                // Get Sample Rates
                snd_pcm_hw_params_get_rate_min(params,&minRate, nullptr);
                snd_pcm_hw_params_get_rate_max(params, &maxRate, nullptr);

                std::vector<unsigned int> rates = {44100, 48000, 96000 };
                for(unsigned int rate : rates) {
                    if (rate >= minRate && rate <= maxRate) {
                        caps.supportedSampleRates.push_back(rate);
                    }
                }

                // Get Buffer Sizes
                uint64_t minBSize;
                uint64_t maxBSize;
                snd_pcm_hw_params_get_buffer_size_min(params, &minBSize);
                snd_pcm_hw_params_get_buffer_size_max(params, &maxBSize);
                std::vector<uint64_t> bSizes {256, 512, 1024, 2048};
                for(auto size : bSizes) {
                    if(size >= minBSize && size <= maxBSize ) {
                        dev.capabilities.supportedBufferSizes.push_back(size);
                    }
                }


                // Test for Interleaved support
                if (snd_pcm_hw_params_test_access(m_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED) == 0) {
                    std::cout << "Device supports Interleaved (LRLR)" << std::endl;
                    caps.supportedBufferFormats.push_back(BufferFormat::Interleaved);
                }

                // Test for Planar (Non-Interleaved) support
                if (snd_pcm_hw_params_test_access(m_handle, params, SND_PCM_ACCESS_RW_NONINTERLEAVED) == 0) {
                    std::cout << "Device supports Planar (LLL RRR)" << std::endl;
                    caps.supportedBufferFormats.push_back(BufferFormat::Planar);

                }


                snd_pcm_close(m_handle);

                // Open the device for capture
                if (snd_pcm_open(&m_handle, dev.UID.c_str(), SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK) < 0) {
                    return; // Device busy or unavailable
                }

                snd_pcm_hw_params_alloca(&params);
                snd_pcm_hw_params_any(m_handle, params);
                
                // Get Channels
                snd_pcm_hw_params_get_channels_max(params, &caps.maxInputChannels);
                snd_pcm_close(m_handle);

                // TODO: Check for full duplex support, as some devices may only allow
                // An input stream or an output stream (half-duplex).
                if(caps.maxInputChannels > 0 && caps.maxOutputChannels > 0) {
                    caps.supportsDuplex = true;
                }

            }
    
            void set_input_device(AudioDevice &dev) override;

            void set_output_device(AudioDevice &dev) override;


            // Sample Rate Management
            void set_sample_rate(int sampleRate) override;
            
            // Buffer Size Management
            void set_buffer_szie(int bufferSize) override;

            // Audio Callback
            void process_audio(AudioBuffer& input, AudioBuffer& output, StreamContext& context ) override;
            
            // Stream Management
            void open_stream() override;
            bool open_playback();
            void close_stream() override;

            void start_stream() override;
            void stop_stream() override;

            // Test sines
            std::vector<float> gen_inter_sine_float(float freq);
            void play_sine(float freq);
        private:
        snd_pcm_t* m_handle = nullptr;
        
    };

    
}


#endif
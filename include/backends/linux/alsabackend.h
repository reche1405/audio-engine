#ifndef ALSABACKEND_H
#define ALSABACKEND_H
#include <vector>
#include <alsa/asoundlib.h>
#include "../audiobackend.h"
#include <string>
#include <iostream>
namespace AudioEngine {
    class ALSABackend : public IAudioBackend {
        public:
            ALSABackend() : IAudioBackend(BackendType::ALSA) {

            }

            std::vector<AudioDevice> enumerate_devices() override {
                /* 
                / Alsa operates different to other audio engines. 
                / The device streams have to be opened as they are iterated through.
                / This allows us to further probe them for information. 
                / As we are testing on WSL, we will have to avoid probing certain 
                / Hardware as it doesn't exist. 
                / 
                / 
                / 
                */

                std::vector<AudioDevice> devices;
                void **hints;

                if (snd_device_name_hint(-1, "pcm", &hints) < 0) return devices;

                for (void **h = hints; *h; ++h) {
                    char *name = snd_device_name_get_hint(*h, "NAME");
                    char *desc = snd_device_name_get_hint(*h, "DESC");
                    char *inOut = snd_device_name_get_hint(*h, "IOID");
                    if (name) {
                        AudioDevice dev;
                        dev.UID = std::string(name);
                        // Use description for UI, fallback to name if NULL
                        dev.name = desc ? std::string(desc) : std::string(name);
                        dev.capabilities.isDefaultInput = dev.name == "default" ? true : false;
                        dev.capabilities.isDefaultOutput = dev.name == "default" ? true : false;
                        probe_device_capabilities(dev);

                        devices.push_back(dev);
                        free(name);
                    }
                    if (desc) free(desc);
                }
                snd_device_name_free_hint(hints);
                return devices;
            }


            void probe_device_capabilities(AudioDevice& dev) {
                snd_pcm_t* handle;
                snd_pcm_hw_params_t* params;

                // Open the device for playback to probe it
                if (snd_pcm_open(&handle, dev.UID.c_str(), SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK) < 0) {
                    return; // Device busy or unavailable
                }

                snd_pcm_hw_params_alloca(&params);
                snd_pcm_hw_params_any(handle, params);

                // Get Supported Sample formats
                if (snd_pcm_hw_params_test_format(handle, params, SND_PCM_FORMAT_FLOAT_LE) == 0) {
                    std::cout << "Device supports S32 Float LE!" << std::endl;

                    dev.capabilities.supportedFormats.push_back(SampleFormat::Float32);
                }

                if (snd_pcm_hw_params_test_format(handle, params, SND_PCM_FORMAT_S16_LE) == 0) {
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
                snd_pcm_close(handle);

                // Open the device for capture
                if (snd_pcm_open(&handle, dev.UID.c_str(), SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK) < 0) {
                    return; // Device busy or unavailable
                }

                snd_pcm_hw_params_alloca(&params);
                snd_pcm_hw_params_any(handle, params);
                
                // Get Channels
                snd_pcm_hw_params_get_channels_max(params, &caps.maxInputChannels);
                snd_pcm_close(handle);
                if(caps.maxInputChannels > 0 && caps.maxOutputChannels > 0) {
                    caps.supportsDuplex = true;
                }
                snd_pcm_close(handle);

            }
    };

    
}


#endif
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
                std::vector<AudioDevice> devices;
                void **hints;

                if (snd_device_name_hint(-1, "pcm", &hints) < 0) return devices;

                for (void **h = hints; *h; ++h) {
                    char *name = snd_device_name_get_hint(*h, "NAME");
                    char *desc = snd_device_name_get_hint(*h, "DESC");
                    char *inOut = snd_device_name_get_hint(*h, "IOID");
                    if (name) {
                        AudioDevice dev;
                        dev.UID = name;
                        // Use description for UI, fallback to name if NULL
                        dev.name = desc ? desc : name;
                       

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



                // Get Channels
                snd_pcm_hw_params_get_channels_max(params, &dev.capabilities.maxOutputChannels);

                unsigned int minRate;
                unsigned int maxRate;

                // Get Sample Rates
                snd_pcm_hw_params_get_rate_min(params,&minRate, nullptr);
                snd_pcm_hw_params_get_rate_max(params, &maxRate, nullptr);

                std::vector<unsigned int> rates = {44100, 48000, 96000 };
                for(unsigned int rate : rates) {
                    if (rate >= minRate && rate <= maxRate) {
                        dev.capabilities.supportedSampleRates.push_back(rate);
                    }
                }
                snd_pcm_close(handle);
            }
    };

    
}


#endif
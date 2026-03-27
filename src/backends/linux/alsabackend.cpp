#include "../include/backends/linux/alsabackend.h"

namespace AudioEngine {
    std::vector<AudioDevice> ALSABackend::enumerate_devices() {
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
                        std::string deviceId(name);

                        // TEMP: Remove all non existant physical devices. *WSL*
                        if (deviceId.find("hw:") == 0 || deviceId.find("plughw:") == 0) {
                            free(name);
                            continue; 
                        }
                        AudioDevice dev;
                        dev.UID = deviceId;
                        dev.name = desc ? std::string(desc) : deviceId;
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


}
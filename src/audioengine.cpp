#include "../include/audioengine.h"
namespace ioengine {
    std::vector<AudioDevice> AudioEngine::list_playback_devices() {
        std::vector<AudioDevice> filtered;
        auto devices = m_backend->list_devices();

        for(auto  dev: devices) {
            std::cout << dev << std::endl;
            if(dev.capabilities.maxOutputChannels > 0) {
                AudioDevice copy;

                memcpy(&copy, &dev, sizeof(dev));
                filtered.push_back(copy);
            }
        }
        return filtered;
    }

    std::vector<AudioDevice> AudioEngine::list_capture_devices() {
        std::vector<AudioDevice> filtered;
        auto devices = m_backend->list_devices();
        for(auto  dev: devices) {
            if(dev.capabilities.maxInputChannels > 0) {
                AudioDevice copy = dev;
                filtered.push_back(copy);
            }
        }
        return filtered;
    }
}
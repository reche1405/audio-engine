#include "../include/audioengine.h"
namespace ioengine {
    std::vector<AudioDevice> AudioEngine::list_playback_devices() {
        std::vector<AudioDevice> filtered;
        auto devices = m_backend->list_devices();

        for(auto  dev: devices) {
            std::cout << dev << std::endl;
            if(dev.capabilities.maxOutputChannels > 0) {
                filtered.push_back(dev);
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
    void AudioEngine::open_stream() {

        m_backend->open_stream();
        printf("Stream opened!");
    }
    void AudioEngine::start_stream() {
        m_backend->start_stream();
        printf("Stream started!");

    }
}
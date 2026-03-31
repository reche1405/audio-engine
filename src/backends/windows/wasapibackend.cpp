#ifdef _WIN32
#ifndef WASAPIBACKEND_H 
#define WASAPIBACKEND_H

#include "./wasapibackend.h"
namespace AudioEngine {
    void WASAPIBackend::initialise() {

    }
    void WASAPIBackend::enumerate_devices() {
        throw new std::runtime_error("Not implemented");
    }
    void WASAPIBackend::set_input_device(AudioDevice& dev) {
        m_captureDevice = dev;
        m_config.inputDeviceName = dev.UID;
    }
    void WASAPIBackend::set_output_device(AudioDevice& dev) {
        m_playbackDevice = dev;
        m_config.outputDeviceName = dev.UID;
    }

     // Sample Rate Management
        void WASAPIBackend::set_sample_rate(int sampleRate)  {
        throw new std::runtime_error("Not implemented");
    };
    
    // Buffer Size Management
        void WASAPIBackend::set_buffer_szie(int bufferSize)  {
        throw new std::runtime_error("Not implemented");
    };

     // Stream Management
    void WASAPIBackend::open_stream()  {
        open_playback();
        //printf("Stream opened");
    };
        void WASAPIBackend::close_stream()  {
            stop_stream();
    };

    void WASAPIBackend::run() {
        throw new std::runtime_error("Not implemented");

    }

    void WASAPIBackend::start_stream()  {
        throw new std::runtime_error("Not implemented");

    }

    void WASAPIBackend::stop_stream()  {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    };

    bool WASAPIBackend::open_playback() {
        throw new std::runtime_error("Not implemented");

    }
    bool WASAPIBackend::open_capture() {
        throw new std::runtime_error("Not implemented");

    }

}
#endif
#endif

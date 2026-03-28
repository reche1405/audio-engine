#include <math.h>
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

                        /*
                        /    TEMP: 
                        /           Remove all non existant physical devices. *WSL*
                        */ 
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
                        if(
                            dev.capabilities.supportedBufferSizes.size() > 0 &&
                            dev.capabilities.supportedSampleRates.size() > 0
                        ){

                            devices.push_back(dev);
                        };   
                        /* TEMP:
                        /      
                        /        If the name is default, we will set the input device and output device
                        / 
                        */

                        if(deviceId == "default") {
                            dev.capabilities.isDefaultInput = true;
                            dev.capabilities.isDefaultOutput = true;
                            set_input_device(dev);
                            set_output_device(dev);
                        };

                        free(name);
                    }
                    if (desc) free(desc);
                }
                snd_device_name_free_hint(hints);
                m_deviceCache = devices;
                return devices;
            }
    
    void ALSABackend::iniitialize() {
        enumerate_devices();
        open_playback();
        
    }
    
    void ALSABackend::set_input_device(AudioDevice &dev) {
        
        m_config.inputDeviceName = dev.UID;
        m_captureDevice = dev;
    };
    
    void ALSABackend::set_output_device(AudioDevice &dev)  {
        m_playbackDevice = dev;
        m_config.outputDeviceName = dev.UID;
    };


    // Sample Rate Management
        void ALSABackend::set_sample_rate(int sampleRate)  {
        throw new std::runtime_error("Not implemented");
    };
    
    // Buffer Size Management
        void ALSABackend::set_buffer_szie(int bufferSize)  {
        throw new std::runtime_error("Not implemented");
    };

    // Audio Callback
        void ALSABackend::process_audio(AudioBuffer& input, AudioBuffer& output, StreamContext& context )  {
        throw new std::runtime_error("Not implemented");
    };
    
    // Stream Management
        void ALSABackend::open_stream()  {
        throw new std::runtime_error("Not implemented");
    };
        void ALSABackend::close_stream()  {
        throw new std::runtime_error("Not implemented");
    };

        void ALSABackend::start_stream()  {
        throw new std::runtime_error("Not implemented");
    };
        void ALSABackend::stop_stream()  {
        throw new std::runtime_error("Not implemented");
    };

    bool ALSABackend::open_playback() {
        snd_pcm_hw_params_t* params;
        snd_pcm_t *handle;
        int err;

        // 1. Open PCM device for playback
        if ((err = snd_pcm_open(&m_handle, m_config.outputDeviceName.value().c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
            std::cerr << "Playback open error: " << snd_strerror(err) << std::endl;
            return false;
        }

        // 2. Allocate and fill hw_params with default values
        snd_pcm_hw_params_alloca(&params);
        snd_pcm_hw_params_any(m_handle, params);

        // 3. Set the hardware parameters
        // Interleaved access (LRLR)
        snd_pcm_hw_params_set_access(m_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
        
        // Signed 16-bit little-endian (Standard)
        snd_pcm_hw_params_set_format(m_handle, params, SND_PCM_FORMAT_FLOAT_LE);
        
        snd_pcm_hw_params_set_channels(m_handle, params, m_config.inputChannels);
        snd_pcm_hw_params_set_rate_near(m_handle, params, &m_config.sampleRate, 0);

        // 4. Write the parameters to the driver
        if ((err = snd_pcm_hw_params(m_handle, params)) < 0) {
            std::cerr << "Unable to set HW parameters: " << snd_strerror(err) << std::endl;
            return false;
        }

        // 5. Prepare the interface
        if ((err = snd_pcm_prepare(m_handle)) < 0) {
            std::cerr << "Prepare error: " << snd_strerror(err) << std::endl;
            return false;
        }

        return true;
    }

    std::vector<float> ALSABackend::gen_inter_sine_float(float freq) {
        int frames = m_config.bufferSize;
        unsigned int sRate = m_config.sampleRate;
        int nChannels = m_config.outputChannels;
        std::vector<float> buffer; 

        buffer.reserve(frames * nChannels);

        static float phase = 0.0f;
        float phaseIncrement = 2.0f * M_PI * freq / sRate;
        float volume = 0.2f; // 20% volume to protect your ears!

        for (int i = 0; i < frames; ++i) {
            float sample = std::sin(phase) * volume;

            // Interleave for ALSA (Left then Right)
            buffer.push_back(sample); 
            buffer.push_back(sample);

            phase += phaseIncrement;
            if (phase >= 2.0f * M_PI) phase -= 2.0f * M_PI;
        }
        return buffer;
    }
    void ALSABackend::play_sine(float freq) {
        while(true) {
            auto buffer = gen_inter_sine_float(freq);
            snd_pcm_sframes_t framesWritten = snd_pcm_writei(m_handle, buffer.data(), m_config.bufferSize);

            // 3. Handle Underruns (The "EPIPE" error)
            if (framesWritten == -EPIPE) {
                // Buffer underrun occurred - the CPU didn't keep up
                snd_pcm_prepare(m_handle);
            } else if (framesWritten < 0) {
                std::cerr << "ALSA Write Error: " << snd_strerror(framesWritten) << std::endl;
            }

        }
    }
}
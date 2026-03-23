#ifndef IAUDIOBACKEND_H
#define IAUDIOBACKEND_H
#include "../common/audioconfig.h"
namespace AudioEngine {
    class IAudioBackend {
        public:
            // List Devices

            virtual void listDevices() = 0;

            virtual DeviceCapabilities getDeviceCapabilities(int deviceId) = 0;
            // Select Devices

            virtual void setInputDevice() = 0;

            virtual void setOutputDevice() = 0;


            // project sample rate
            virtual void setSampleRate(int sampleRate) = 0;


            // Audio Callback

            virtual void processAudio() = 0;
            
            // Stream Management
            virtual void openStream(StreamConfig config) = 0;
            virtual void closeStream() = 0;

            virtual void startStream() = 0;
            virtual void stopStream() = 0;

            // Latency Management
            virtual void setLatency(int latency) = 0;


        // private:

    };
}

#endif 
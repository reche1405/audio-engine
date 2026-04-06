#ifdef _WIN32
#ifndef WASAPIBACKEND_H
#define WASAPIBACKEND_H

#include "../audiobackend.h"
#include "../../common/audioconfig.h"
#include <mmdeviceapi.h>
#include <propidlbase.h>
#include <wrl/client.h>
namespace ioengine {
    class WASAPIBackend : public IAudioBackend
    {
        private:
            Microsoft::WRL::ComPtr<IMMDeviceEnumerator> m_enum;
            Microsoft::WRL::ComPtr<IMMDeviceCollection> m_col;
            IAudioClient *m_client;
            IAudioRenderClient *m_renderClient;
            HRESULT m_hr; 
            HANDLE m_buffEvent;

        public:
            WASAPIBackend() : IAudioBackend(BackendType::WASAPI) {

            }
            void iniitialize() override;
            
            std::vector<AudioDevice> enumerate_devices() override;


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

            void process_audio(float *,float *,StreamContext &) override;
            void exit_on_error();

            // Internal Management
            std::string to_std_string(PROPVARIANT *var) {
                std::wstring ws(var->pwszVal);
                std::string str(ws.begin(), ws.end());
                return str;
            }
            bool is_default_device(EDataFlow *flow, LPWSTR *id);
        
    };
}

#endif
#endif
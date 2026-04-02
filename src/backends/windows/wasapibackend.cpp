#ifdef _WIN32

#include "../../../include/backends/windows/wasapibackend.h"
#include <string>
#include <AudioClient.h>
#include <mmdeviceapi.h>
#include <combaseapi.h>
#include <Functiondiscoverykeys_devpkey.h>
namespace AudioEngine {
    void WASAPIBackend::iniitialize() {
        HRESULT init = CoInitializeEx(
            NULL,
            COINIT_MULTITHREADED
        );
        if(init != S_OK) {
            throw new std::runtime_error("Unable to initialise the backend.");
        }  
        enumerate_devices();
    }
    std::vector<AudioDevice> WASAPIBackend::enumerate_devices() {
        std::vector<AudioDevice> devices;
        IMMDeviceEnumerator *pEnumerator = NULL;

        IMMDevice *pDevice = NULL;
        const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
        const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
        IPropertyStore *pProps = NULL;
        LPWSTR pwszID = NULL;
        UINT count;
        auto hr = CoCreateInstance(
         CLSID_MMDeviceEnumerator, NULL,
         CLSCTX_ALL, IID_IMMDeviceEnumerator,
         (void**)&pEnumerator);

         if(FAILED(hr)) {
            throw new std::runtime_error("Unable to enumerate devices.");
         }

        IMMDeviceCollection *pCollection = NULL;
        hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
        
        hr = pCollection->GetCount(&count);
        std::cout << "Total Count of Multimedia devices" << count << std::endl;
        if(count == 0) {
            throw new std::runtime_error("No audio endpoints found.");
        }


        for(ULONG i = 0; i < count; i++) {
            AudioDevice dev;
            hr = pCollection->Item(i, &pDevice);

            hr = pDevice->GetId(&pwszID);
            std::wstring ws(pwszID);
            std::string strId(ws.begin(), ws.end());
            dev.UID = strId;
            hr = pDevice->OpenPropertyStore(
                          STGM_READ, &pProps);
            if(FAILED(hr)) {
                throw new std::runtime_error("Unable to open protperty store.");
            }

            PROPVARIANT varName;
            // Initialize container for property value.
            PropVariantInit(&varName);

            // Get the endpoint's friendly-name property.
            hr = pProps->GetValue(
                        PKEY_Device_FriendlyName, &varName);
            if(FAILED(hr)) {
                throw new std::runtime_error("Unable to enumerate devices.");
            } 
            if (varName.vt != VT_EMPTY) {
                // Print endpoint friendly name and endpoint ID.
                
                std::string _name = to_std_string(varName);
                dev.name = _name;
                printf("Endpoint %d: \"%S\" (%S)\n", 
                        i, varName.pwszVal, pwszID);
            }
            PropVariantClear(&varName);
            
            
            
            IAudioClient *client = nullptr; 
            pDevice->Activate(
                __uuidof(IAudioClient),
                CLSCTX_ALL,
                NULL,
                (void**)&client
            );

            WAVEFORMATEX *pDevFormat; 
            // Backend buffer size is set and cannot be changed.
            int64_t pDefaultPeriod;
            int64_t pMinPeriod;
            // Buffer period is measured in hns (hundred nano seconds)
            // To get the defaut buffer frames it is (hns * sample rate) / 10_000_000
            client->GetMixFormat(&pDevFormat);
            client->GetDevicePeriod(&pDefaultPeriod, &pMinPeriod);
            uint32_t defaultSRate = pDevFormat->nSamplesPerSec;
            dev.capabilities.supportedSampleRates.push_back(defaultSRate);
            std::cout << "The device period in hns: " << pDefaultPeriod << std::endl;
            unsigned int _bufferFrames = (pDefaultPeriod * defaultSRate) / 10000000;
            dev.capabilities.supportedBufferSizes.push_back(_bufferFrames);
            std::cout << "The average device period in frames: " << _bufferFrames << std::endl;

            bool isFloat32 = pDevFormat->wFormatTag == WAVE_FORMAT_PCM;
            std::cout <<  "Wave format:" << pDevFormat->wFormatTag << std::endl;
            pDevice->Release();
            pDevice = nullptr;
            client->Release();
            client = nullptr;
    

            devices.push_back(dev);
        }
        if (pCollection) pCollection->Release();
        if (pEnumerator) pEnumerator->Release();
    return devices;
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

    void WASAPIBackend::process_audio(float *,float *,StreamContext &) {

    }

}
#endif

#ifdef _WIN32

#include "../../../include/backends/windows/wasapibackend.h"
#include <string>
#include <AudioClient.h>
#include <mmdeviceapi.h>
#include <Ksmedia.h>
#include <combaseapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <guiddef.h>
namespace ioengine {
    void WASAPIBackend::iniitialize() {
        std::cout << "Initialising WASAPI Interface" << std::endl;
        HRESULT init = CoInitializeEx(
            NULL,
            COINIT_MULTITHREADED
        );
        if(init != S_OK) {
            throw new std::runtime_error("Unable to initialise the backend.");
        }  
        printf("Beginning enumeration.\n");
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
        m_hr = CoCreateInstance(
         CLSID_MMDeviceEnumerator, NULL,
         CLSCTX_ALL, IID_IMMDeviceEnumerator,
         (void**)&m_enum);

         if(FAILED(m_hr)) {
            throw new std::runtime_error("Unable to enumerate devices.");
         }

        IMMDeviceCollection *pCollection = NULL;
        m_hr = m_enum->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &m_col);
        
        m_hr = m_col->GetCount(&count);
        if(count == 0) {
            throw new std::runtime_error("No audio endpoints found.");
        }

        IMMEndpoint *pEndpoint = nullptr;
        EDataFlow *flow = nullptr;

        for(ULONG i = 0; i < count; i++) {
            AudioDevice dev;
            dev.capabilities.supportedBufferFormats.push_back(BufferFormat::Interleaved);
            m_hr = m_col->Item(i, &pDevice);
            m_hr = pDevice->QueryInterface(
                __uuidof(IMMEndpoint),
                (void**)&pEndpoint
            );
            
            m_hr = pDevice->GetId(&pwszID);
            std::wstring ws(pwszID);
            std::string strId(ws.begin(), ws.end());
            dev.UID = strId;
            m_hr = pDevice->OpenPropertyStore(
                          STGM_READ, &pProps);
            if(FAILED(m_hr)) {
                throw new std::runtime_error("Unable to open protperty store.");
            }

            PROPVARIANT varName;
            // Initialize container for property value.
            PropVariantInit(&varName);

            // Get the endpoint's friendly-name property.
            m_hr = pProps->GetValue(
                        PKEY_Device_FriendlyName, &varName);
            if(FAILED(m_hr)) {
                throw new std::runtime_error("Unable to enumerate devices.");
            } 
            if (varName.vt != VT_EMPTY) {
                // Print endpoint friendly name and endpoint ID.
                
                std::string _name = to_std_string(&varName);
                dev.name = _name;
                // std::cout << dev.name << std::endl;
                
            }
            PropVariantClear(&varName);
                       
            IAudioClient *client = nullptr; 
            pDevice->Activate(
                __uuidof(IAudioClient),
                CLSCTX_ALL,
                NULL,
                (void**)&client
            );

            m_hr = pEndpoint->GetDataFlow(flow);
            
            WAVEFORMATEX *pDevFormat; 
            // Backend buffer size is set and cannot be changed.
            int64_t pDefaultPeriod;
            int64_t pMinPeriod;
            client->GetMixFormat(&pDevFormat);
            int channels = pDevFormat->nChannels;
            

            std::cout << "Endpoint"  << i<<": " <<  dev.name << std::endl;
            
            // Buffer period is measured in hns (hundred nano seconds)
            // To get the defaut buffer frames it is (hns * sample rate) / 10_000_000
            client->GetDevicePeriod(&pDefaultPeriod, &pMinPeriod);
            uint32_t defaultSRate = pDevFormat->nSamplesPerSec;
            dev.capabilities.supportedSampleRates.push_back(defaultSRate);
            std::cout << "The device period in hns: " << pDefaultPeriod << std::endl;
            unsigned int _bufferFrames = (pDefaultPeriod * defaultSRate) / 10000000;
            dev.capabilities.supportedBufferSizes.push_back(_bufferFrames);
            std::cout << "The average device period in frames: " << _bufferFrames << std::endl;

            if(pDevFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE  &&
            pDevFormat->cbSize >= (sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX))) {
                
                //WAVEFORMATEXTENSIBLE *wForm = (WAVEFORMATEXTENSIBLE *) pDevFormat->wFormatTag;
                
                auto wForm = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pDevFormat);
                    std::cout << wForm << std::endl;
                 if(IsEqualGUID(wForm->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)) {
                    dev.capabilities.supportedFormats.push_back(SampleFormat::Float32);
                    std::cout <<  "-    Supports Float 32" << std::endl;
                    
                } 
                std::cout <<  "Wave format: Extensible" << std::endl;
            }

            if((int) flow == eRender) {
                std::cout << "Device Channels: " << channels << std::endl;
                dev.capabilities.maxOutputChannels = (unsigned int) channels;   
                if(is_default_device(flow,&pwszID)) {
                    dev.capabilities.isDefaultOutput = true;
                    set_output_device(dev);
                } 
            }
            else if((int) flow == eCapture) {
                dev.capabilities.maxInputChannels = (unsigned int) channels;   
                if(is_default_device(flow,&pwszID)) {
                    dev.capabilities.isDefaultInput = true;
                    set_input_device(dev);
                } 
            }

            devices.push_back(dev);
            m_deviceCache.push_back(dev);
            std::cout << "Total devices: " << devices.size() << std::endl;

            
            client->Release();
            client = nullptr;
            
        }
        pDevice->Release();
        pDevice = nullptr;
        pEndpoint->Release();
        pEndpoint = nullptr;
        if (pCollection) pCollection->Release();
        if (pEnumerator) pEnumerator->Release();
    return devices;
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
        if (m_running) return;

        m_running = true;
        std::thread(&WASAPIBackend::run, this);
   
        printf("Thread opened.\n");

        throw new std::runtime_error("Not implemented");

    }

    void WASAPIBackend::stop_stream()  {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    };

    bool WASAPIBackend::open_playback() {
         REFERENCE_TIME hnsRequestedDuration = 10000000;
        REFERENCE_TIME hnsActualDuration;
        IMMDevice * pDevice; 
        IAudioClient * pClient;
        IAudioRenderClient *pRenderClient = NULL;
        UINT32 bufferFrameCount;
        BYTE *pData;
        WAVEFORMATEX *pwfx;
        // playback device Id.
        std::wstring wtemp = std::wstring(m_playbackDevice.UID.begin(), m_playbackDevice.UID.end());
        LPCWSTR wstr = wtemp.c_str();
        m_hr = m_enum->GetDevice(wstr, &pDevice);
        exit_on_error();
        m_hr = pDevice->Activate(__uuidof(IAudioClient),
             CLSCTX_ALL,
             NULL,
             (void**)&pClient    
        );
        exit_on_error();

        m_hr = pClient->GetMixFormat(&pwfx);
        exit_on_error();

        m_hr = pClient->Initialize(
            AUDCLNT_SHAREMODE_SHARED,
            0,
            hnsRequestedDuration,
            0,
            pwfx,
            NULL
        );
        exit_on_error();

        m_hr = pClient->GetBufferSize(&bufferFrameCount);
        exit_on_error();

        m_hr = pClient->GetService(
            __uuidof(IAudioRenderClient),
            (void**)&pRenderClient
        );
        exit_on_error();


        m_hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);

        exit_on_error();

        float *fData = reinterpret_cast<float*>(pData);
        std::vector<float> initBuffer;
        initBuffer.resize(bufferFrameCount * 2);
        std::fill(initBuffer.begin(), initBuffer.end(), 0.0f);
        std::memcpy(fData, initBuffer.data(), bufferFrameCount * 2);
        //m_listener->ring_buffer()->popBlock(bufferFrameCount * 2)
        m_hr = pRenderClient->ReleaseBuffer(bufferFrameCount, 0);
        hnsActualDuration = (double)10000000 *
            bufferFrameCount / pwfx->nSamplesPerSec;
        exit_on_error();
        m_hr = pClient->Start();
        exit_on_error();
        
    }
    bool WASAPIBackend::open_capture() {
        throw new std::runtime_error("Not implemented");

    }

    void WASAPIBackend::process_audio(float *,float *,StreamContext &) {

    }
    void WASAPIBackend::exit_on_error() {
        if(FAILED(m_hr)) {
            throw new std::runtime_error("Backend Interaction failure.");
        }
    }

    bool WASAPIBackend::is_default_device(EDataFlow *flow, LPWSTR *id) {
        IMMDevice* pDefaultDevice = nullptr;
        LPWSTR *defaultID = nullptr;
        if((int) &flow == 0) {
            m_hr = m_enum->GetDefaultAudioEndpoint(eRender, eConsole, &pDefaultDevice);
            if (SUCCEEDED(m_hr)) {
                pDefaultDevice->GetId(defaultID);
                return wcscmp(*id, *defaultID) == 0;
                
            }
        } else if((int) &flow == 1) {
            m_hr = m_enum->GetDefaultAudioEndpoint(eCapture, eConsole, &pDefaultDevice);
            if (SUCCEEDED(m_hr)) {
                pDefaultDevice->GetId(defaultID);
                return wcscmp(*id, *defaultID) == 0;
                
            }
        }
        return false;
    }

}
#endif

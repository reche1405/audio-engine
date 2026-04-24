#ifdef _WIN32

#include "../../../include/backends/windows/wasapibackend.h"
#include <string>
#include <AudioClient.h>
#include <avrt.h>
#include <mmdeviceapi.h>
#include <Ksmedia.h>
#include <combaseapi.h>
#include <Functiondiscoverykeys_devpkey.h>
#include <guiddef.h>
#include <locale>
#include <codecvt>
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
         (void**)&m_enum
        );

        if(FAILED(m_hr)) {
            throw new std::runtime_error("Unable to enumerate devices.");
        }

        IMMDeviceCollection *pCollection = NULL;
        m_hr = m_enum->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &m_col);
            exit_on_error("Unable to enumerate endpoints.");
        
        m_hr = m_col->GetCount(&count);
        if(count == 0) {
            throw new std::runtime_error("No audio endpoints found.");
        }

        IMMEndpoint *pEndpoint = nullptr;
        
        for(ULONG i = 0; i < count; i++) {
            EDataFlow flow;
            AudioDevice dev;
            dev.capabilities.supportedBufferFormats.push_back(BufferFormat::Interleaved);
            m_hr = m_col->Item(i, &pDevice);
            m_hr = pDevice->QueryInterface(
                __uuidof(IMMEndpoint),
                (void**)&pEndpoint
            );
            exit_on_error("Unable to Query Interface.");
            
            m_hr = pDevice->GetId(&pwszID);
            exit_on_error("Unable to Query Device ID");

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

            m_hr = pEndpoint->GetDataFlow(&flow);
            exit_on_error("Data flow error");
            std::cout << "Data Flow: " << flow << std::endl;
            WAVEFORMATEX *pDevFormat; 
            // Backend buffer size is set and cannot be changed.
            int64_t pDefaultPeriod;
            int64_t pMinPeriod;
            client->GetMixFormat(&pDevFormat);
            int channels = pDevFormat->nChannels;
            

            // std::cout << "Endpoint"  << i<<": " <<  dev.name << std::endl;
            
            // Buffer period is measured in hns (hundred nano seconds)
            // To get the defaut buffer frames it is (hns * sample rate) / 10_000_000
            client->GetDevicePeriod(&pDefaultPeriod, &pMinPeriod);
            uint32_t defaultSRate = pDevFormat->nSamplesPerSec;
            dev.capabilities.supportedSampleRates.push_back(defaultSRate);
            // std::cout << "The device period in hns: " << pDefaultPeriod << std::endl;
            unsigned int _bufferFrames = (pDefaultPeriod * defaultSRate) / 10000000;
            dev.capabilities.supportedBufferSizes.push_back(_bufferFrames);
            // std::cout << "The average device period in frames: " << _bufferFrames << std::endl;

            if(pDevFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE  &&
            pDevFormat->cbSize >= (sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX))) {
                
                //WAVEFORMATEXTENSIBLE *wForm = (WAVEFORMATEXTENSIBLE *) pDevFormat->wFormatTag;
                
                auto wForm = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(pDevFormat);
                    // std::cout << wForm << std::endl;
                 if(IsEqualGUID(wForm->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)) {
                    dev.capabilities.supportedFormats.push_back(SampleFormat::Float32);
                    // std::cout <<  "-    Supports Float 32" << std::endl;
                    
                } 
                // std::cout <<  "Wave format: Extensible" << std::endl;
            }

            if(flow ==  EDataFlow::eRender) {
                std::cout << "Device Channels: " << channels << std::endl;
                dev.capabilities.maxOutputChannels = (unsigned int) channels;   

                if(is_default_device(flow,&pwszID)) {
                    std::cout << "Default playback device found" << std::endl;
                    dev.capabilities.isDefaultOutput = true;
                    set_output_device(dev);
                } 
            }
            else if(flow == EDataFlow::eCapture) {
                std::cout << "Device Channels: " << channels << std::endl;
                dev.capabilities.maxInputChannels = (unsigned int) channels;   
                if(is_default_device(flow,&pwszID)) {
                    dev.capabilities.isDefaultInput = true;
                    set_input_device(dev);
                } 
            }

            devices.push_back(dev);
            m_deviceCache.push_back(dev);
            // std::cout << "Total devices: " << devices.size() << std::endl;

            
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

        printf("Stream opened");
    };

    void WASAPIBackend::close_stream()  {
        stop_stream();
        CoUninitialize();

        m_renderClient->Release();
        exit_on_error();
        
        m_client->Release();
        exit_on_error();

        m_enum->Release();
        exit_on_error();

    };

    void WASAPIBackend::run() {
        DWORD taskIndex = 0;
        // Register the thread as "Pro Audio"
        LPCWSTR cTaskStr = L"Pro Audio";
        HANDLE hTask = AvSetMmThreadCharacteristicsW(cTaskStr, &taskIndex);
        if (hTask == NULL) {
            DWORD error = GetLastError();
            std::cerr << "HTASK is null: " << error << std::endl;
        } else {
            AvSetMmThreadPriority(hTask, AVRT_PRIORITY_HIGH);
            SetThreadAffinityMask(GetCurrentThread(), 1 << 1);
        }

        UINT32 numFramesAvailable;
        UINT32 numFramesPadding;
        UINT32 bufferFrameCount;
        m_hr = m_client->GetBufferSize(&bufferFrameCount);
        BYTE *pData;
        while(m_running) {
            DWORD waitResult = WaitForSingleObject(m_buffEvent, 200);   
            if (waitResult == WAIT_OBJECT_0) {
                // GetBuffer, Process, and ReleaseBuffer logic goes here
                
                m_hr = m_client->GetCurrentPadding(&numFramesPadding);
                if(FAILED(m_hr)) {
                    std::cout << "Unable to get current padding" << std::endl;
                }
                numFramesAvailable = bufferFrameCount - numFramesPadding;

                m_hr = m_renderClient->GetBuffer(numFramesAvailable, &pData);
                if(FAILED(m_hr)) {
                    std::cout << "Unable to get buffer" << std::endl;
                }
                float *fData = reinterpret_cast<float*>(pData);
                UINT32 framesInRingBuffer = m_listener->ring_buffer().availableSamples() / 2;
                UINT32 framesToWrite = min(numFramesAvailable, framesInRingBuffer);
                int framesWritten = m_listener->ring_buffer().popBlock(fData, framesToWrite * 2) / 2;
                if (framesWritten < numFramesAvailable) {
                    // Fill remaining frames with silence
                    int silenceFrames = numFramesAvailable - framesWritten;
                    float* silenceStart = fData + (framesWritten * 2);  // Stereo interleaved
                    memset(silenceStart, 0, silenceFrames * 2 * sizeof(float));
                }
                m_hr = m_renderClient->ReleaseBuffer(numFramesAvailable, 0);

                
            } else if (waitResult == WAIT_TIMEOUT || waitResult == WAIT_ABANDONED) {
                DWORD error = GetLastError();
                std::cerr << "Wait Failed: " << error <<  std::endl;
                continue;
            }
             else {
                continue;
            }
        }
        AvRevertMmThreadCharacteristics(hTask);

    }

    void WASAPIBackend::start_stream()  {
        if (m_running) return;

        m_running = true;
        m_thread = std::thread(&WASAPIBackend::run, this);
   
        printf("Thread opened.\n");

        //throw new std::runtime_error("Not implemented");

    }

    void WASAPIBackend::stop_stream()  {
        m_running = false;
        SetEvent(m_buffEvent);
        if (m_thread.joinable()) {
            m_thread.join();
        }
        CloseHandle(m_buffEvent);

        m_hr = m_client->Stop();
        exit_on_error();

    };

    bool WASAPIBackend::open_playback() {
        std::cout << "Opening Playback" << std::endl;
        REFERENCE_TIME hnsRequestedDuration = 10000000;
        REFERENCE_TIME hnsActualDuration;
        IMMDevice * pDevice; 
        IAudioClient * pClient;
        IAudioRenderClient *pRenderClient;
        UINT32 bufferFrameCount;
        BYTE *pData;
        WAVEFORMATEX *pwfx;
        // playback device Id.
         m_buffEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if(m_buffEvent == NULL) {

            return false;
        }
       std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wtemp = converter.from_bytes(m_playbackDevice.UID);
        m_hr = m_enum->GetDevice(wtemp.c_str(), &pDevice);
        std::cout << m_playbackDevice.UID << std::endl;

        exit_on_error("Unable to get device");
        m_hr = pDevice->Activate(__uuidof(IAudioClient),
             CLSCTX_ALL,
             NULL,
             (void**)&m_client    
        );
        exit_on_error("Unable to Activate Device");
        std::cout << "Device  activated" << std::endl;
        m_hr = m_client->GetMixFormat(&pwfx);
        exit_on_error("Unable to get Mix Format");

        m_hr = m_client->Initialize(
            AUDCLNT_SHAREMODE_SHARED,
            AUDCLNT_STREAMFLAGS_EVENTCALLBACK,  
            hnsRequestedDuration,
            0,
            pwfx,
            NULL
        );
        exit_on_error("Unable to initialise the client.");

        m_hr = m_client->GetBufferSize(&bufferFrameCount);
        exit_on_error("Unable to get buffersize.");
        m_bufferSize = (int) bufferFrameCount;
        m_hr = m_client->GetService(
            __uuidof(IAudioRenderClient),
            (void**)&m_renderClient
        );
        exit_on_error("Unable to obtain render client refeerence.");


        m_hr = m_renderClient->GetBuffer(bufferFrameCount, &pData);

        exit_on_error("Unable to fain a reference to the buffer data container.");

        float *fData = reinterpret_cast<float*>(pData);
        std::vector<float> initBuffer;
        initBuffer.resize(bufferFrameCount * 2);
        std::fill(initBuffer.begin(), initBuffer.end(), 0.0f);
         m_listener->ring_buffer().popBlock(fData, bufferFrameCount * 2);
        //m_listener->ring_buffer()->popBlock(fData, bufferFrameCount * 2)
        m_hr = m_renderClient->ReleaseBuffer(bufferFrameCount, 0);
        exit_on_error("Error releasing buffer");
        hnsActualDuration = (double)10000000 *
            bufferFrameCount / pwfx->nSamplesPerSec;

       
        m_client->SetEventHandle(m_buffEvent);

        m_hr = m_client->Start();
        exit_on_error("Unable to start the playback client.");
        std::cout << "Device Has been started" << std::endl;
        return true;
    }
    bool WASAPIBackend::open_capture() {
        throw new std::runtime_error("Not implemented");

    }

    void WASAPIBackend::process_audio(float *,float *,StreamContext &) {

    }
    void WASAPIBackend::exit_on_error(std::string message) {
        if(FAILED(m_hr)) {
            std::cout << message << std::endl;
            exit(1);
            throw new std::runtime_error("Backend Interaction failure.");
        }
    }

    bool WASAPIBackend::is_default_device(EDataFlow flow, LPWSTR *id) {
        std::cout << "checking for default." << std::endl;
        IMMDevice* pDefaultDevice = nullptr;
        LPWSTR defaultID;
        if(flow == EDataFlow::eCapture) {
            m_hr = m_enum->GetDefaultAudioEndpoint(EDataFlow::eCapture, eConsole, &pDefaultDevice);
            if (SUCCEEDED(m_hr)) {
                m_hr = pDefaultDevice->GetId(&defaultID);
                exit_on_error("Unable to get default capture device ID.");
                return wcscmp(*id, defaultID) == 0;
                
            }
        } else if(flow == EDataFlow::eRender) {
            m_hr = m_enum->GetDefaultAudioEndpoint(EDataFlow::eRender, eConsole, &pDefaultDevice);
            exit_on_error("Unable to get default render device.");
            if (SUCCEEDED(m_hr)) {
                pDefaultDevice->GetId(&defaultID);
                return wcscmp(*id, defaultID) == 0;
                
            }
        }
        if (pDefaultDevice) {
            pDefaultDevice->Release();
            pDefaultDevice = nullptr;
        }
        if(defaultID) {
            defaultID = nullptr;
        }
        return false;
    }

}
#endif

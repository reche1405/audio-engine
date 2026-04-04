#ifndef AUDIOERROR_H
#define AUDIOERROR_H
#include <stdexcept>
#include <string>

namespace ioengine {
enum class AudioErrorCode {
    Success = 0,
    DeviceNotFound,
    DeviceUnavailable,
    InvalidConfiguration,
    SampleRateUnsupported,
    BufferSizeUnsupported,
    AudioBackendInitFailed,
    AudioBackendStartFailed,
    AudioBackendStopFailed,
    RealTimePriorityFailed,
    AudioCallbackError,
    AudioStreamClosed,
    PlatformSpecificError
};

class AudioException : public std::runtime_error {
public:
    AudioException(AudioErrorCode code, const std::string& message)
        : std::runtime_error(message), m_code(code) {}

    AudioErrorCode code() const { return m_code; }
private:
    AudioErrorCode m_code;
};

class DeviceException : public AudioException {
    using AudioException::AudioException;
};

class DeviceNotFoundException : public DeviceException {
    public:
        DeviceNotFoundException(const std::string& deviceName) : 
        DeviceException(AudioErrorCode::DeviceNotFound, "Unable to find device: " + deviceName),
        m_deviceName(deviceName) {}

    private: 
        std::string m_deviceName;
};

class DeviceUnavailableException : public DeviceException {
    public:
        DeviceUnavailableException(const std::string& deviceName) : 
        DeviceException(AudioErrorCode::DeviceUnavailable, "This device is unavailable:  " + deviceName),
        m_deviceName(deviceName) {}

    private: 
        std::string m_deviceName;
};

class InvalidConfigException : public DeviceException {
    public:
    InvalidConfigException(const std::string& deviceName ) :
    DeviceException(AudioErrorCode::InvalidConfiguration, "Invalid configuration for " + deviceName),
    m_deviceName(deviceName) {}

    private: 
        std::string m_deviceName;
};

class SampleRateUnsupportedException : public DeviceException {
    public:
    SampleRateUnsupportedException(const std::string& deviceName) :
    DeviceException(AudioErrorCode::SampleRateUnsupported, "Unsupported sample rate for " + deviceName),
    m_deviceName(deviceName) {};

    private:
        std::string m_deviceName;
};

class BufferSizeUnsupportedException : public DeviceException {
    public:
    BufferSizeUnsupportedException(const std::string& deviceName) :
    DeviceException(AudioErrorCode::BufferSizeUnsupported, "Unsupported buffer size for " + deviceName),
    m_deviceName(deviceName) {};

    private:
        std::string m_deviceName;
};


}



#endif // AUDIOERROR_H

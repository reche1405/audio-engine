#ifndef AUDIOCONFIG_H
#define AUDIOCONFIG_H

#include <cstdint>
#include <string>
#include <vector>
#include <optional>



namespace AudioEngine {

    const int DEFAULT_SAMPLE_RATE = 48000;
    const int DEFAULT_BUFFER_SIZE = 512;
    const int DEFAULT_CHANNELS = 2;
// Audio format support
enum class SampleFormat {
    Float32,    // 32-bit float (most common)
    Int16,      // 16-bit integer (CD quality)
    Int24,      // 24-bit integer (pro audio)
    Int32       // 32-bit integer
};

enum class BufferFormat {
    Planar, 
    Interleaved
};

// Buffer behavior
enum class BufferStrategy {
    Fixed,      // Fixed buffer size (simpler)
    Adaptive,   // Adapt to system capabilities
    LowLatency, // Minimum possible latency
    Stable      // Maximum stability (larger buffers)
};

// Platform-specific backend types
enum class BackendType {
    Auto,       // Auto-select best backend
    ASIO,       // Windows ASIO
    WASAPI,     // Windows WASAPI
    DirectSound,// Windows DirectSound
    CoreAudio,  // macOS CoreAudio
    JACK,       // Linux JACK
    ALSA,       // Linux ALSA
    Pulse,      // Linux PulseAudio
    RtAudio     // RtAudio wrapper (our default)
};

// Device capabilities
struct DeviceCapabilities {
    std::vector<unsigned int> supportedSampleRates;
    std::vector<int> supportedBufferSizes;
    std::vector<SampleFormat> supportedFormats;
    int maxInputChannels;
    int maxOutputChannels;
    bool supportsInput;
    bool supportsOutput;
    bool supportsDuplex;
    double minLatencyMs;
    double maxLatencyMs;
    bool isDefaultInput;
    bool isDefaultOutput;
};

// Audio configuration for a stream
struct StreamConfig {
    // Device selection
    std::optional<std::string> inputDeviceName;
    std::optional<std::string> outputDeviceName;

    // Stream parameters
    int sampleRate = DEFAULT_SAMPLE_RATE;
    int bufferSize = DEFAULT_BUFFER_SIZE;     // Frames per buffer
    int inputChannels = DEFAULT_CHANNELS;
    int outputChannels = DEFAULT_CHANNELS;

    // Format
    SampleFormat sampleFormat = SampleFormat::Float32;
    BufferFormat BufferFormat = BufferFormat::Planar;
    // Behavior
    BufferStrategy bufferStrategy = BufferStrategy::Stable;
    bool allowSampleRateChange = false;
    bool allowBufferSizeChange = false;
    bool exclusiveMode = false;  // Exclusive hardware access

    // Platform specific
    BackendType preferredBackend = BackendType::Auto;

    // Validation
    bool isValid() const;
    std::string toString() const;
};

struct StreamContext {
    double sampleRate;
    uint64_t streamTime;
    bool isSilence;
};

// Latency measurements
struct LatencyInfo {
    double theoreticalMs;     // bufferSize / sampleRate * 1000
    double measuredMs;        // Actual measured round-trip
    double jitterMs;          // Variation in latency
    double cpuUsage;          // CPU usage percentage
    int xruns;                // Buffer over/under runs
};

}
#endif

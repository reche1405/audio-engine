#ifndef AUDIOCONFIG_H
#define AUDIOCONFIG_H

#include <cstdint>
#include <string>
#include <vector>
#include <optional>

#include <iostream>
#include <iomanip>

namespace AudioEngine {

    const unsigned int DEFAULT_SAMPLE_RATE = 48000;
    const uint64_t DEFAULT_BUFFER_SIZE = 512;
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
    std::vector<uint64_t> supportedBufferSizes;
    std::vector<unsigned int> supportedPeriodSizes;

    std::vector<SampleFormat> supportedFormats;
    std::vector<BufferFormat> supportedBufferFormats;
    unsigned int maxInputChannels;
    unsigned maxOutputChannels;
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
    unsigned int  sampleRate = DEFAULT_SAMPLE_RATE;
    uint64_t bufferSize = DEFAULT_BUFFER_SIZE;     // Frames per buffer
    unsigned int inputChannels = DEFAULT_CHANNELS;
    unsigned int outputChannels = DEFAULT_CHANNELS;

    // Format
    SampleFormat sampleFormat = SampleFormat::Float32;
    BufferFormat bufferFormat = BufferFormat::Interleaved;
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

struct TestTone {
    TestTone() {};
    ~TestTone() = default;
        float phase = 0.0f; 
        float frequency = 440.0f; // A4 note
        float sampleRate = DEFAULT_SAMPLE_RATE;
        float amplitude = 0.5f; 
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

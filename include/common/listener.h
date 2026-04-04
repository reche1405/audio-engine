#ifndef LISTENER_H
#define LISTENER_H
#include <cstdint>
#include "./ringbuffer.h"
namespace ioengine {
    struct Listener {
    
    virtual void on_buffer_request(float* buffer, uint32_t frames) = 0;
    virtual RingBuffer<float> &ring_buffer() = 0;
};

}
#endif
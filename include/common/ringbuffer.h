#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <vector>
#include <atomic>
#include <cstdint>

namespace AudioEngine {
    template <typename T>
    class RingBuffer{
         private:
        alignas(64) std::atomic<size_t> m_head;
        alignas(64) std::atomic<size_t> m_tail;
        size_t m_capacity;
        size_t m_mask;
        std::vector<T> m_buffer;



    public:
        explicit RingBuffer(size_t capacity) {
            m_capacity = capacity;
            m_mask = capacity - 1;
            m_buffer.resize(m_capacity);
            m_head.store(0);
            m_tail.store(0);
        };
        bool push(T& data) {
            size_t head = m_head.load(std::memory_order_relaxed);
            size_t nextHead = (head + 1) & m_mask;

            // If the buffer is full
            if(nextHead == m_tail.load(std::memory_order_acquire)) {
                return false;
            }
            m_buffer[head] = data;
            m_head.store(nextHead,std::memory_order_release);
            return true;

        };
        bool pop(T& data) {
            size_t tail = m_tail.load(std::memory_order_relaxed);
            if(tail == m_head.load(std::memory_order_acquire)) {
                return false;
            }
            data = m_buffer[tail];
            m_tail.store((tail + 1) & m_mask, std::memory_order_release);
            return true;
        }

        // Pushes multiple samples into the buffer
        size_t pushBlock(const T* data, size_t count) {
            size_t head = m_head.load(std::memory_order_relaxed);
            size_t tail = m_tail.load(std::memory_order_acquire);

            // Calculate how much space is actually available
            size_t available = (tail - head - 1) & m_mask;
            size_t toWrite = std::min(count, available);
            if(toWrite == 0) return 0;
            size_t firstPart = std::min(toWrite, m_capacity - head);
            std::memcpy(&m_buffer[head], data, firstPart * sizeof(T));

            // 4. Second copy (The wrap-around)
            if (toWrite > firstPart) {
                std::memcpy(&m_buffer[0], data + firstPart, (toWrite - firstPart) * sizeof(T));
            }

            m_head.store((head + toWrite) & m_mask, std::memory_order_release);
            return toWrite; // Return how many we actually managed to push
        }

        // Pops multiple samples out of the buffer
        size_t popBlock(T* data, size_t count) {
            size_t head = m_head.load(std::memory_order_acquire);
            size_t tail = m_tail.load(std::memory_order_relaxed);

            size_t available = (head - tail) & m_mask;
            size_t toRead = std::min(count, available);
            if(toRead == 0) return 0;
            size_t firstPart = std::min(toRead, m_capacity - tail);
            std::memcpy(data, &m_buffer[tail], firstPart * sizeof(T));
            if (toRead > firstPart) {
                // Wrap around: copy the remaining from the start
                std::memcpy(data + firstPart, &m_buffer[0], (toRead - firstPart) * sizeof(T));
            }

            m_tail.store((tail + toRead) & m_mask, std::memory_order_release);
            return toRead; // Return how many we actually popped
        }

        size_t availableSamples() const {
            size_t head = m_head.load(std::memory_order_acquire);
            size_t tail = m_tail.load(std::memory_order_acquire);
            return (head - tail) & m_mask;
        }
    };
}
#endif
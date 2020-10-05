/*
    File        ringbuffer.h
    Author      Heiko Klausing (h.klausing at gmx dot de)
    Created     2020-07-31

    Note        Data can be stored endless to this buffer. The data
                type is user definable.
    Feature list
        - data type defined by user
        - buffer limit 65536
        - no write stop, read pointer will be incremented by each read
        - write pointer will be incremented by each write
        - buffer full detection
        - write pointer moves read pointer if buffer is full marked
        - content returns the amount of defined buffer elements
        - size returns the amount of buffered elements
        - Full state: m_full is true, if m_head changes to 0
        - Empty state: (m_head == 0) && !full
        - Reading of data has no effect to pointer, read data will not deleted
        - Buffer has to be filled with dummy values
        - Buffer size must be known at compile time

    Usage:  const size_t BUFSIZE = 32;
            typedef struct {
                uint32 value1;
                uint32 value2;
            } data_t;
            RingBuffer<data_t, BUFSIZE> ringbuffer(data_t(0,0));
            ringbuffer.add(data_t(1,2));
            ringbuffer.add(data_t(3,4));
*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>


template <typename _T, size_t _NSIZE>
class RingBuffer
{
private:
    /* Limits */
    static const size_t MAX_BUFFER_LENGTH = 0xffff;
    /* data */
    size_t      m_head;         // write pointer, points to next write index
    bool        m_full;         // status flag
    _T          m_buffer[_NSIZE];   // buffer for data

    // NOTE: disable constructor, copy constructor and assignment operator
    RingBuffer(void) = delete;
    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) & = delete;

public:
    RingBuffer(_T dummy)
        : m_head(0)
        , m_full(false)
    {
        static_assert(_NSIZE > 0 && _NSIZE <= MAX_BUFFER_LENGTH, "size must be in range 1..65500");
    };

    ~RingBuffer(){};

    // no element written to the buffer
    bool isEmpty()
    {
        return !m_full && !m_head;
    };

    // Returns true if the buffer is full, minimum _N elements written to the buffer
    // false if the less than _N elements wriiten to the buffer
    bool isFull()
    {
        return m_full;
    };

    // clears the buffer and all control elements
    void clear()
    {
        m_head = 0;
        m_full = false;
    }

    // returns the amount of defined buffer elements
    size_t content()
    {
        return _NSIZE;
    }

    // returns the amount of stored buffer elements
    size_t size()
    {
        if(isFull())
            return _NSIZE;
        return m_head;
    }

    // add the next element to the buffer
    void add(const _T data) {

        // add data ..
        m_buffer[m_head] = data;

        // .. and adjust the pointer and flags
        m_head = (m_head + 1) % _NSIZE;
        if(m_head == 0) {
            m_full = true;
        }
    }

    // read values from the newest to be oldest
    // offest = 0 -> last element
    // offest = 1 -> element before last element
    // offset = ...
    _T& readLast(size_t offset=0)
    {
        //assert(!isEmpty());
        size_t offsetValue = offset % _NSIZE;

        if(m_full)
            return m_buffer[((m_head + _NSIZE) - 1 - offsetValue) % _NSIZE];
        else {
            assert(offsetValue < m_head );
        }
        return m_buffer[m_head - 1 - offsetValue];
    }

    // read elements from the oldest to the newest
    _T& readFirst(size_t offset=0)
    {
        //assert(!isEmpty());
        size_t offsetValue = offset % _NSIZE;

        if(!m_full) {
            assert(offsetValue < m_head);
            return m_buffer[offsetValue];
        }

        return m_buffer[(m_head + offsetValue) % _NSIZE];
    }

};

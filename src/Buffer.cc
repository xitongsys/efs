#include <cstring>
#include <iostream>

#include "Buffer.h"

namespace efs {
Buffer::Buffer(int32_t capacity)
{
    buffer = new char[capacity];
    this->capacity = capacity;
    write_pos = 0;
    read_pos = 0;
}

Buffer::~Buffer()
{
    delete buffer;
}

int32_t Buffer::read_size()
{
    if (read_pos > write_pos) {
        return capacity - read_pos + write_pos;
    }
    return write_pos - read_pos;
}

int32_t Buffer::write_size()
{
    if (read_pos > write_pos) {
        return read_pos - write_pos - 1;
    }

    return capacity - write_pos + read_pos - 1;
}

void Buffer::read_consume(int32_t size)
{
    read_pos = (read_pos + size) & ((capacity - 1));
}

void Buffer::write_consume(int32_t size)
{
    write_pos = (write_pos + size) & ((capacity - 1));
}

void Buffer::read(char* data, int32_t size)
{
    std::memcpy(data, buffer + read_pos, size);
    read_consume(size);
}

void Buffer::write(const char* data, int32_t size)
{
    std::memcpy(buffer + write_pos, data, size);
    write_consume(size);
}

const char* Buffer::read_raw_buffer()
{
    return buffer + read_pos;
}

char* Buffer::write_raw_buffer()
{
    return buffer + write_pos;
}

}
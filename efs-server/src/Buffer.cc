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
    end_pos = 0;
}

Buffer::Buffer()
{
    buffer = nullptr;
    this->capacity = 0;
    write_pos = 0;
    read_pos = 0;
    end_pos = 0;
}

Buffer::~Buffer()
{
    if (buffer) {
        delete[] buffer;
    }
}

int32_t Buffer::read_size()
{
    if (read_pos > write_pos) {
        return end_pos - read_pos;
    }
    return write_pos - read_pos;
}

int32_t Buffer::write_size()
{
    if (read_pos > write_pos) {
        return read_pos - write_pos - 1;
    }

    if (read_pos - 1 >= capacity - write_pos) {
        return read_pos - 1;

    } else {
        return capacity - write_pos;
    }
}

const char* Buffer::read_raw_buffer()
{
    return buffer + read_pos;
}

char* Buffer::write_raw_buffer()
{
    if (read_pos > write_pos) {
        return buffer + write_pos;
    }

    if (read_pos - 1 >= capacity - write_pos) {
        end_pos = write_pos;
        write_pos = 0;
    }

    return buffer + write_pos;
}

void Buffer::read_consume(int32_t size)
{
    if (read_pos > write_pos) {
        read_pos = (read_pos + size);
        if (read_pos >= end_pos) {
            read_pos = 0;
            end_pos = 0;
        }

    } else {
        read_pos = (read_pos + size);
        if (read_pos >= write_pos) {
            write_pos = 0;
            read_pos = 0;
            end_pos = 0;
        }
    }
}

void Buffer::write_consume(int32_t size)
{
    write_pos = (write_pos + size);
}

}
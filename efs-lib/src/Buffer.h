#pragma once

#include <stdint.h>

namespace efs {
class Buffer {
public:
    char* buffer;
    int32_t capacity;
    int32_t write_pos, read_pos;
    int32_t end_pos;

public:
    int32_t read_size();
    int32_t write_size();

    void write_consume(int32_t n);
    void read_consume(int32_t n);

    const char* read_raw_buffer();
    char* write_raw_buffer();

public:
    Buffer();
    Buffer(int32_t size);
    ~Buffer();
};
}
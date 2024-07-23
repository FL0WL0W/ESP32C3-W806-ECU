#include "stdint.h"
#include "stdlib.h"

#ifndef SERIAL_H
#define SERIAL_H

typedef uint32_t StreamSpeed;

class Stream
{
    public:
    void begin(StreamSpeed);
    uint8_t read();
    uint8_t peek();
    int8_t available();
    int8_t availableForWrite();
    int8_t write(uint8_t);
    int8_t write(uint8_t *, size_t);
    void flush();
    void print(const char c);
    void print(const char *c);
    void println();
    template<typename K>
    void println(K k)
    {

    }
};

extern Stream Serial;

#endif
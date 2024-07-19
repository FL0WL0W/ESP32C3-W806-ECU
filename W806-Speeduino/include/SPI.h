#ifndef SPI_H
#define SPI_H

#include "stdint.h"

typedef uint32_t SPISpeed;
enum SPIMode : uint8_t
{
    SPI_MODE0 = 0,
    SPI_MODE1 = 1,
    SPI_MODE2 = 2,
    SPI_MODE3 = 3
};
struct SPISettings
{
    SPISettings(SPISpeed speed, uint8_t bitFirst, SPIMode mode)
    {
        
    }
};

class SPIClass
{
    public:
    void begin();
    void beginTransaction(SPISettings settings);
    uint8_t transfer16(uint16_t);
};

static SPIClass SPI;

#endif
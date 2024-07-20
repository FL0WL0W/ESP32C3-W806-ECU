#include "stdlib.h"
#include "stdint.h"

#ifndef W806EEPROM_H
#define W806EEPROM_H

class W806EEPROM
{
    public:
    size_t length();

    uint8_t read(size_t);

    int8_t write(size_t, uint8_t);

    int8_t update(size_t, uint8_t);

    template< typename T > T &get( int idx, T &t ){
        uint16_t e = idx;
        uint8_t *ptr = (uint8_t*) &t;
        for( int count = sizeof(T) ; count ; --count, ++e )  *ptr++ = read(e);
        return t;
    }

    template< typename T > const T &put( int idx, const T &t ){        
        const uint8_t *ptr = (const uint8_t*) &t;
        uint16_t e = idx;
        for( int count = sizeof(T) ; count ; --count, ++e )  write(e, *ptr++);
        return t;
    }
};

extern W806EEPROM EEPROM;

#endif
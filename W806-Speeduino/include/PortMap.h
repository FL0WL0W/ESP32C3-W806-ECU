#ifndef PORTMAP_H
#define PORTMAP_H

#ifdef __cplusplus
#include "stdint.h"
#include "IDigitalService.h"

extern EmbeddedIOServices::IDigitalService *Arduino_DigitalService;
struct GPIOPort_EIOS
{
    uint8_t _pinNumber;
    GPIOPort_EIOS(uint8_t pinNumber) : _pinNumber(pinNumber) {}

    volatile void operator=(volatile uint8_t pinMask) volatile
    {
        if(pinMask & 0x1)
            Arduino_DigitalService->WritePin(_pinNumber, true);
        else
            Arduino_DigitalService->WritePin(_pinNumber, false);
    }
    volatile uint8_t operator|(volatile uint8_t pinMask) volatile
    {
        return pinMask | Arduino_DigitalService->ReadPin(_pinNumber);
    }
    volatile uint8_t operator&(volatile uint8_t pinMask) volatile
    {
        return pinMask & Arduino_DigitalService->ReadPin(_pinNumber);
    }
    volatile void operator|=(volatile uint8_t pinMask) volatile
    {
        if(!(pinMask & 0x1))
            return;
        Arduino_DigitalService->WritePin(_pinNumber, true);
    }
    volatile void operator&=(volatile uint8_t pinMask) volatile
    {
        if(pinMask & 0x1)
            return;
        Arduino_DigitalService->WritePin(_pinNumber, false);
    }
    volatile void operator^=(volatile uint8_t pinMask) volatile
    {
        if(!(pinMask & 0x1))
            return;
        Arduino_DigitalService->WritePin(_pinNumber, Arduino_DigitalService->ReadPin(_pinNumber));
    }
};

GPIOPort_EIOS *portOutputRegister(uint8_t pin);

#endif
#endif
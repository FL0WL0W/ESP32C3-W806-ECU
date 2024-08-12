#include "Arduino.h"

EmbeddedIOServices::ITimerService *Arduino_TimerService;
EmbeddedIOServices::IDigitalService *Arduino_DigitalService;
EmbeddedIOServices::IAnalogService *Arduino_AnalogService;
EmbeddedIOServices::IPwmService *Arduino_PwmService;

uint32_t millis(void)
{
    if(Arduino_TimerService == 0)
        return 0;
    return Arduino_TimerService->GetTick() / (Arduino_TimerService->GetTicksPerSecond() / 1000.0f);
}

uint32_t micros(void)
{
    if(Arduino_TimerService == 0)
        return 0;
    return Arduino_TimerService->GetTick() / (Arduino_TimerService->GetTicksPerSecond() / 1000000.0f);
}

void delay(unsigned long ms)
{
    const EmbeddedIOServices::tick_t startTick = Arduino_TimerService->GetTick();
    const EmbeddedIOServices::tick_t endTick = startTick + ms * (Arduino_TimerService->GetTicksPerSecond() / 1000.0f);
    while(EmbeddedIOServices::ITimerService::TickLessThanTick(Arduino_TimerService->GetTick(), endTick)) ;
}

void pinMode(uint8_t pin, uint8_t mode)
{
    switch(mode)
    {
        case INPUT:
            Arduino_DigitalService->InitPin(pin, EmbeddedIOServices::In);
            break;
        case INPUT_PULLUP:
            Arduino_DigitalService->InitPin(pin, EmbeddedIOServices::In);
            break;
        case INPUT_ANALOG:
            Arduino_AnalogService->InitPin(pin);
            break;
        case OUTPUT:
            Arduino_DigitalService->InitPin(pin, EmbeddedIOServices::Out);
        break;
    }
}

void digitalWrite(uint8_t pin, bool val)
{
    Arduino_DigitalService->WritePin(pin, val);
}

bool digitalRead(uint8_t pin) 
{
    return Arduino_DigitalService->ReadPin(pin);
}

const uint32_t analogResolution = 1024;
int analogRead(uint8_t pin)
{
    return Arduino_AnalogService->ReadPin(pin) * analogResolution;
}
float analogPeriod = 1.0f/500;
void analogWrite(uint8_t pin, int val)
{
    Arduino_PwmService->WritePin(pin, { analogPeriod, analogPeriod * val / analogResolution});
}

void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode)
{
    Arduino_DigitalService->AttachInterrupt(interruptNum, userFunc);
}

void detachInterrupt(uint8_t interruptNum)
{
    Arduino_DigitalService->DetachInterrupt(interruptNum);
}
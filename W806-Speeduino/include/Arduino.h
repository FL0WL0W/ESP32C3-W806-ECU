#ifndef Arduino_h
#define Arduino_h

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <avr/pgmspace.h>

#define ATOMIC_BLOCK(P) P
#define ATOMIC_RESTORESTATE 

#ifdef __cplusplus
#include "Serial.h"
#include "ITimerService.h"
#include "IDigitalService.h"
#include "IAnalogService.h"
#include "IPwmService.h"
#include "bits/stl_algobase.h"
#include "PortMap.h"
using namespace std;
extern EmbeddedIOServices::ITimerService *Arduino_TimerService;
extern EmbeddedIOServices::IDigitalService *Arduino_DigitalService;
extern EmbeddedIOServices::IAnalogService *Arduino_AnalogService;
extern EmbeddedIOServices::IPwmService *Arduino_PwmService;
extern "C"{
#else
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#define F(P) P
#define memcpy_P memcpy

void yield(void);

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define INPUT_ANALOG 0x3

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif

#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define interrupts() //asm("sei")
#define noInterrupts() //asm("cli")

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitToggle(value, bit) ((value) ^= (1UL << (bit)))
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))

// avr-libc defines _NOP() since 1.6.2
#ifndef _NOP
#define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

typedef unsigned int word;

#define bit(b) (1UL << (b))

typedef bool boolean;
typedef uint8_t byte;

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, bool val);
bool digitalRead(uint8_t pin);
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int val);

uint32_t millis(void);
uint32_t micros(void);
void delay(unsigned long ms);

void attachInterrupt(uint8_t interruptNum, void (*userFunc)(void), int mode);
void detachInterrupt(uint8_t interruptNum);

void setup(void);
void loop(void);

#define digitalPinToPort(P) P
#define digitalPinToBitMask(P) 0x1
#define portInputRegister portOutputRegister
#define portModeRegister(P) 0

#define NOT_A_PIN 0
#define NOT_A_PORT 0

#define NOT_AN_INTERRUPT -1

#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus

uint16_t makeWord(uint16_t w);
uint16_t makeWord(byte h, byte l);

#define word(...) makeWord(__VA_ARGS__)

// WMath prototypes
long map(long, long, long, long, long);

#endif

#include "pins_arduino.h"

#endif
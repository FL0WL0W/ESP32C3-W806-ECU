#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include "stdint.h"

#define NUM_DIGITAL_PINS            70
#define NUM_ANALOG_INPUTS           16
#define analogInputToDigitalPin(p)  ((p < 16) ? (p) + 54 : -1)
#define digitalPinHasPWM(p)         (((p) >= 2 && (p) <= 13) || ((p) >= 44 && (p)<= 46))

#define PIN_SPI_SS    (53)
#define PIN_SPI_MOSI  (51)
#define PIN_SPI_MISO  (50)
#define PIN_SPI_SCK   (52)

static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#define PIN_WIRE_SDA        (20)
#define PIN_WIRE_SCL        (21)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

#define LED_BUILTIN 13

#define PIN_A0   (3)
#define PIN_A1   (49)
#define PIN_A2   (50)
#define PIN_A3   (51)
#define PIN_A4   (52)
#define PIN_A5   (53)
#define PIN_A6   (54)
#define PIN_A7   (55)
#define PIN_A8   (56)
#define PIN_A9   (57)
#define PIN_A10  (58)
#define PIN_A11  (17)
#define PIN_A12  (18)
#define PIN_A13  (19)
#define PIN_A14  (20)
#define PIN_A15  (21)
#define PIN_A16  (22)
#define PIN_A17  (23)
#define PIN_A18  (24)
#define PIN_A19  (25)
#define PIN_A20  (26)
#define PIN_A21  (1)
#define PIN_A22  (2)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;
static const uint8_t A6 = PIN_A6;
static const uint8_t A7 = PIN_A7;
static const uint8_t A8 = PIN_A8;
static const uint8_t A9 = PIN_A9;
static const uint8_t A10 = PIN_A10;
static const uint8_t A11 = PIN_A11;
static const uint8_t A12 = PIN_A12;
static const uint8_t A13 = PIN_A13;
static const uint8_t A14 = PIN_A14;
static const uint8_t A15 = PIN_A15;
static const uint8_t A16 = PIN_A16;
static const uint8_t A17 = PIN_A17;
static const uint8_t A18 = PIN_A18;
static const uint8_t A19 = PIN_A19;
static const uint8_t A20 = PIN_A20;
static const uint8_t A21 = PIN_A21;
static const uint8_t A22 = PIN_A22;

#define digitalPinToInterrupt(p) p

#endif
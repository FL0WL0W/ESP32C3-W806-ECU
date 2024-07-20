#include "Arduino.h"

uint32_t millis(void)
{
    return 1234;
}

uint32_t micros(void)
{
    return 12345678;
}

void digitalWrite(uint8_t pin, uint8_t val)
{

}

uint32_t *const PROGMEM port_to_mode_PGM[] = {
};

uint32_t *const PROGMEM port_to_output_PGM[] = {
};

uint32_t *const PROGMEM port_to_input_PGM[] = {
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
};

const uint32_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
};
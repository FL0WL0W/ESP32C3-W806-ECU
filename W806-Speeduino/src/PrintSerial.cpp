
#include "PrintSerial.h"

// PrintSerial Serial;

void PrintSerial::begin(unsigned long baud, int uart_mode)
{
}

void PrintSerial::begin()
{
}

void PrintSerial::begin(unsigned long baud)
{
}
int PrintSerial::read(void)
{
    return 0;
}
int PrintSerial::peek()
{
    return 0;
}
size_t PrintSerial::write(uint8_t c)
{
    printf("%c", c);
}

size_t PrintSerial::write(const uint8_t *buffer, size_t size)
{
    return printf("%.*s", size, buffer);;
}

int PrintSerial::available(void)
{
    return 0;
}

int PrintSerial::printf(const char *fmt, ...)
{
    va_list args;
    int len;
    va_start(args, fmt);

    return printf(fmt, args);
}
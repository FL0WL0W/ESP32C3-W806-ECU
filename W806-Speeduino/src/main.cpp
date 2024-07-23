/******************************************************************************
** 
 * \file        main.c
 * \author      IOsetting | iosetting@outlook.com
 * \date        
 * \brief       Demo code of PWM in independent mode
 * \note        This will drive 3 on-board LEDs to show fade effect
 * \version     v0.1
 * \ingroup     demo
 * \remarks     test-board: HLK-W806-KIT-V1.0
 *              PWM Frequency = 40MHz / Prescaler / (Period + 1)；
                Duty Cycle(Edge Aligned)   = (Pulse + 1) / (Period + 1)
                Duty Cycle(Center Aligned) = (2 * Pulse + 1) / (2 * (Period + 1))
 *
******************************************************************************/

#include <stdio.h>
#include "wm_hal.h"

#include "Arduino.h"
#include "DigitalService_EG14.h"
#include "AnalogService_EG14.h"
#include "TimerService_W80x.h"

EmbeddedIOServices::ATTiny427Expander_Registers _registersU1;
EmbeddedIOServices::ATTiny427Expander_Registers _registersU2;
EmbeddedIOServices::ATTiny427ExpanderUpdateService *_updateServiceU1;
EmbeddedIOServices::ATTiny427ExpanderUpdateService *_updateServiceU2;

EmbeddedIOServices::DigitalService_W80x *_digitalService_W80x;
EmbeddedIOServices::DigitalService_ATTiny427Expander *_digitalService_U1;
EmbeddedIOServices::DigitalService_ATTiny427Expander *_digitalService_U2;
EmbeddedIOServices::DigitalService_EG14 *_digitalService;

EmbeddedIOServices::AnalogService_W80x *_analogService_W80x;
EmbeddedIOServices::AnalogService_ATTiny427Expander *_analogService_U1;
EmbeddedIOServices::AnalogService_ATTiny427Expander *_analogService_U2;
EmbeddedIOServices::AnalogService_EG14 *_analogService;

EmbeddedIOServices::TimerService_W80x *_timerService;

//undefined function so defining it. not sure WTF in csky libstdc++ is requiring this function, but here it is
extern "C" size_t write(int fd, const void *buf, size_t count)
{
    return -1;
}

extern "C" int main(void)
{
    SystemClock_Config(CPU_CLK_240M);
    printf("enter main\r\n");

    _updateServiceU1 = new EmbeddedIOServices::ATTiny427ExpanderUpdateService(&_registersU1);
    _updateServiceU2 = new EmbeddedIOServices::ATTiny427ExpanderUpdateService(&_registersU2);

    _digitalService_W80x = new EmbeddedIOServices::DigitalService_W80x();
    _digitalService_U1 = new EmbeddedIOServices::DigitalService_ATTiny427Expander(&_registersU1);
    _digitalService_U2 = new EmbeddedIOServices::DigitalService_ATTiny427Expander(&_registersU2);
    _digitalService = new EmbeddedIOServices::DigitalService_EG14(_digitalService_W80x, _digitalService_U1, _digitalService_U2);

    _analogService_W80x = new EmbeddedIOServices::AnalogService_W80x();
    _analogService_U1 = new EmbeddedIOServices::AnalogService_ATTiny427Expander(&_registersU1);
    _analogService_U2 = new EmbeddedIOServices::AnalogService_ATTiny427Expander(&_registersU2);
    _analogService = new EmbeddedIOServices::AnalogService_EG14(_analogService_W80x, _analogService_U1, _analogService_U2);

    _timerService = new EmbeddedIOServices::TimerService_W80x(1,0);

    Arduino_DigitalService = _digitalService;
    Arduino_AnalogService = _analogService;
    Arduino_TimerService = _timerService;

    setup();

    while (1)
    {
        loop();
    }
}

extern "C" void Error_Handler(void)
{
    while (1)
    {
    }
}

extern "C" void assert_failed(uint8_t *file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}
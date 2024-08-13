#include "AnalogService_ATTiny427Expander.h"

#ifdef ANALOGSERVICE_ATTINY427EXPANDER_H
namespace EmbeddedIOServices
{
	AnalogService_ATTiny427Expander::AnalogService_ATTiny427Expander(ATTiny427Expander_Registers *registers) : _registers(registers)
    {
        _registers->AnalogAccumulate = 0xF; // Accumulate to 16 bits. might adjust this later on.
    }

	void AnalogService_ATTiny427Expander::InitPin(analogpin_t pin)
	{
        const AnalogChannel_ATTiny427Expander AnalogChannel = PinToAnalogChannel(pin);
        if(AnalogChannel > 7)
        {
            if(AnalogChannel < 5)
            {
                if(_registers->Comm == SPI)
                    return;
                if(AnalogChannel < 3)
                {
                    if(_registers->Comm == UART0Alternate || _registers->Comm == UART1)
                        return;
                }
            }
            _registers->AnalogEnable1 |= 0x1 << (AnalogChannel - 8);
        }
        else
        {
            if(AnalogChannel >11)
            {
                if(_registers->Comm == SPIAlternate)
                    return;
                if(_registers->Comm == UART1Alternate && (AnalogChannel == 12 || AnalogChannel == 13))
                    return;
            }
            _registers->AnalogEnable0 |= 0x1 << AnalogChannel;
        }
	}
	
	float AnalogService_ATTiny427Expander::ReadPin(analogpin_t pin)
	{
        const AnalogChannel_ATTiny427Expander AnalogChannel = PinToAnalogChannel(pin);
        uint8_t index = 0;
        for(uint8_t i = 1; i < 8 && i < AnalogChannel; i++)
            if(_registers->AnalogEnable0 & 0x1 << i) index++;
        for(uint8_t i = 0; i < 8 && i < AnalogChannel - 8; i++)
            if(_registers->AnalogEnable0 & 0x1 << i) index++;
        return _registers->AnalogValues[index] * (5.0f / (((2^12) * _registers->AnalogAccumulate) - 1));
	}
}
#endif

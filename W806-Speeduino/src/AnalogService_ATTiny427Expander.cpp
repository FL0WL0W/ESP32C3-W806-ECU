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
            if(AnalogChannel < 5 && !_registers->AlternateSPI)
                return;
            _registers->AnalogEnable1 |= 0x1 << (AnalogChannel - 8);
        }
        else
        {
            if(AnalogChannel >11 && _registers->AlternateSPI)
                return;
            _registers->AnalogEnable0 |= 0x1 << AnalogChannel;
        }
	}
	
	float AnalogService_ATTiny427Expander::ReadPin(analogpin_t pin)
	{
        const AnalogChannel_ATTiny427Expander AnalogChannel = PinToAnalogChannel(pin);
        return _registers->AnalogValues[AnalogChannel - (!_registers->AlternateSPI? 5 : 0)] * (5.0f / ((2^_registers->AnalogAccumulate) - 1));
	}
}
#endif

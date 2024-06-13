#include "DigitalService_ATTiny427Expander.h"

#ifdef DIGITALSERVICE_ATTINY427EXPANDER_H
namespace EmbeddedIOServices
{
    DigitalService_ATTiny427Expander::DigitalService_ATTiny427Expander(ATTiny427Expander_Registers *registers) : _registers(registers)
    {
    }
	void DigitalService_ATTiny427Expander::InitPin(digitalpin_t pin, PinDirection direction)
	{
		const DigitalPort_ATTiny427Expander DigitalPort = PinToDigitalPort(pin);
		const DigitalPin_ATTiny427Expander DigitalPin = PinToDigitalPin(pin);
		switch(DigitalPort)
		{
			case PORTA:
				if(direction == In)
					_registers->PORTA_DIR &= ~DigitalPin;
				else
					_registers->PORTA_DIR |= DigitalPin;
				return;
			case PORTB:
				if(direction == In)
					_registers->PORTB_DIR &= ~DigitalPin;
				else
					_registers->PORTB_DIR |= DigitalPin;
				return;
			case PORTC:
				if(direction == In)
					_registers->PORTC_DIR &= ~DigitalPin;
				else
					_registers->PORTC_DIR |= DigitalPin;
				return;
		}
	}
	bool DigitalService_ATTiny427Expander::ReadPin(digitalpin_t pin)
	{
		const DigitalPort_ATTiny427Expander DigitalPort = PinToDigitalPort(pin);
		const DigitalPin_ATTiny427Expander DigitalPin = PinToDigitalPin(pin);
		switch(DigitalPort)
		{
			case PORTA:
				return _registers->PORTA_IN & DigitalPin;
			case PORTB:
				return _registers->PORTB_IN & DigitalPin;
			case PORTC:
				return _registers->PORTC_IN & DigitalPin;
		}
	}
	void DigitalService_ATTiny427Expander::WritePin(digitalpin_t pin, bool value)
	{
		const DigitalPort_ATTiny427Expander DigitalPort = PinToDigitalPort(pin);
		const DigitalPin_ATTiny427Expander DigitalPin = PinToDigitalPin(pin);
		switch(pin>>3)
		{
			case PORTA:
				if(value == false)
					_registers->PORTA_OUT &= ~DigitalPin;
				else
					_registers->PORTA_OUT |= DigitalPin;
				return;
			case PORTB:
				if(value == false)
					_registers->PORTB_OUT &= ~DigitalPin;
				else
					_registers->PORTB_OUT |= DigitalPin;
				return;
			case PORTC:
				if(value == false)
					_registers->PORTC_OUT &= ~DigitalPin;
				else
					_registers->PORTC_OUT |= DigitalPin;
				return;
		}
	}
	void DigitalService_ATTiny427Expander::AttachInterrupt(digitalpin_t pin, callback_t callBack)
	{
		const DigitalPort_ATTiny427Expander DigitalPort = PinToDigitalPort(pin);
		const DigitalPin_ATTiny427Expander DigitalPin = PinToDigitalPin(pin);
		_interruptList.push_front(DigitalInterrupt_ATTiny427Expander(DigitalPort, DigitalPin, callBack));

	}
	void DigitalService_ATTiny427Expander::DetachInterrupt(digitalpin_t pin)
	{
		const DigitalPort_ATTiny427Expander DigitalPort = PinToDigitalPort(pin);
		const DigitalPin_ATTiny427Expander DigitalPin = PinToDigitalPin(pin);
		_interruptList.remove_if([DigitalPort, DigitalPin](const DigitalInterrupt_ATTiny427Expander& interrupt) { return interrupt.DigitalPort == DigitalPort && interrupt.DigitalPin == DigitalPin; });
	}
	bool DigitalService_ATTiny427Expander::InitPassthrough(digitalpin_t pinIn, digitalpin_t pinOut, bool inverted)
	{
		switch(pinOut)
		{
			case 2:
				//EVOUTA
				break;
			case 4:
				//LUT0OUT
				break;
			case 5:
			{
				DeinitPassthrough(pinIn);
				//primarily use ACOUT since it does not have any alternates and doesn't use any EventChannels
				if(pinIn == 7 || pinIn == 13 || pinIn == 9 || pinIn == 14)
				{
					_registers->AC_DACREF = 0x85;
					_registers->AC_MUXCTRLA = (inverted? 0b10000000 : 0) | 0x3;
					switch(pinIn)
					{
						case 13:
							_registers->AC_MUXCTRLA |= 0x1 << 3;;
							break;
						case 9:
							_registers->AC_MUXCTRLA |= 0x2 << 3;
							break;
						case 14:
							_registers->AC_MUXCTRLA |= 0x3 << 3;
							break;
					}
					_registers->AC_CTRLA = 0b01000111; //enable with large hysteresis and output
				}
				//otherwise use LUT3ALT, if LUT3 was already being used, it will be overwritten
				else
				{
					//disable AC if it is enabled
					_registers->AC_CTRLA = 0;

					//check if matching a LUT3 input pin
					if(pinIn > 15 && pinIn < 19)
					{

					}
					//otherwise we have to use an EventChannel
					else
					{

					}
				}
				return false;
			}
			case 7:
				break;
			case 10:
				break;
			case 11:
				break;
			case 12:
				break;
			case 14:
				break;
			case 15:
				break;
			case 17:
				break;
			case 18:
				break;
			case 20:
				break;
		}
		return false;
	}
	void DigitalService_ATTiny427Expander::DeinitPassthrough(digitalpin_t pinOut)
	{
		switch(pinOut)
		{
			case 2:
				//EVOUTA
				break;
			case 4:
				//LUT0OUT
				break;
			case 5:
				//disable AC if it is enabled
				_registers->AC_CTRLA = 0;
				break;
			case 7:
				break;
			case 10:
				break;
			case 11:
				break;
			case 12:
				break;
			case 14:
				break;
			case 15:
				break;
			case 17:
				break;
			case 18:
				break;
			case 20:
				break;
		}
	}
	void DigitalService_ATTiny427Expander::Update()
	{
		_previousPORTA_IN ^= _registers->PORTA_IN;
		_previousPORTB_IN ^= _registers->PORTB_IN;
		_previousPORTC_IN ^= _registers->PORTC_IN;

		for (DigitalInterruptList_ATTiny427Expander::iterator interrupt = _interruptList.begin(); interrupt != _interruptList.end(); ++interrupt)
		{
			const uint8_t DigitalPin = PinToDigitalPin(interrupt->DigitalPin);
			switch(interrupt->DigitalPort)
			{
				case PORTA: 
						if(_previousPORTA_IN & DigitalPin && (_registers->AlternateSPI || DigitalPin > 4))
							interrupt->CallBack();
					break;
				case PORTB: 
						if(_previousPORTB_IN& DigitalPin)
							interrupt->CallBack();
					break;
				case PORTC: 
						if(_previousPORTC_IN & DigitalPin && (!_registers->AlternateSPI || DigitalPin > 3))
							interrupt->CallBack();
					break;
			}
		}

		_previousPORTA_IN = _registers->PORTA_IN;
		_previousPORTB_IN = _registers->PORTB_IN;
		_previousPORTC_IN = _registers->PORTC_IN;
	}
}
#endif

#include "DigitalService_ATTiny427Expander.h"

#ifdef DIGITALSERVICE_ATTINY427EXPANDER_H
namespace EmbeddedIOServices
{
    DigitalService_ATTiny427Expander::DigitalService_ATTiny427Expander(bool altSPI) : _altSPI(altSPI)
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
					_newRegisters.PORTA_DIR &= ~DigitalPin;
				else
					_newRegisters.PORTA_DIR |= DigitalPin;
				return;
			case PORTB:
				if(direction == In)
					_newRegisters.PORTB_DIR &= ~DigitalPin;
				else
					_newRegisters.PORTB_DIR |= DigitalPin;
				return;
			case PORTC:
				if(direction == In)
					_newRegisters.PORTC_DIR &= ~DigitalPin;
				else
					_newRegisters.PORTC_DIR |= DigitalPin;
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
				return _newRegisters.PORTA_IN & DigitalPin;
			case PORTB:
				return _newRegisters.PORTB_IN & DigitalPin;
			case PORTC:
				return _newRegisters.PORTC_IN & DigitalPin;
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
					_newRegisters.PORTA_OUT &= ~DigitalPin;
				else
					_newRegisters.PORTA_OUT |= DigitalPin;
				return;
			case PORTB:
				if(value == false)
					_newRegisters.PORTB_OUT &= ~DigitalPin;
				else
					_newRegisters.PORTB_OUT |= DigitalPin;
				return;
			case PORTC:
				if(value == false)
					_newRegisters.PORTC_OUT &= ~DigitalPin;
				else
					_newRegisters.PORTC_OUT |= DigitalPin;
				return;
		}
	}
	void DigitalService_ATTiny427Expander::AttachInterrupt(digitalpin_t pin, callback_t callBack)
	{
		const DigitalPort_ATTiny427Expander DigitalPort = PinToDigitalPort(pin);
		const DigitalPin_ATTiny427Expander DigitalPin = PinToDigitalPin(pin);
		InterruptList.push_front(DigitalInterrupt_ATTiny427Expander(DigitalPort, DigitalPin, callBack));

	}
	void DigitalService_ATTiny427Expander::DetachInterrupt(digitalpin_t pin)
	{
		const DigitalPort_ATTiny427Expander DigitalPort = PinToDigitalPort(pin);
		const DigitalPin_ATTiny427Expander DigitalPin = PinToDigitalPin(pin);
		InterruptList.remove_if([DigitalPort, DigitalPin](const DigitalInterrupt_ATTiny427Expander& interrupt) { return interrupt.DigitalPort == DigitalPort && interrupt.DigitalPin == DigitalPin; });
	}
	void DigitalService_ATTiny427Expander::ConfigurePassthrough(digitalpin_t pinIn, digitalpin_t pinOut, bool inverted)
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
				//ACOUT
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
	size_t DigitalService_ATTiny427Expander::Update(uint8_t inOutBuffer[22])
	{
		uint8_t bufferIndex = 0;
		if(!_operations.First) 
		{
			if(_operations.PORTC_OUT)
				bufferIndex+=3;
			if(_operations.PORTB_OUT)
				bufferIndex+=_operations.PORTC_DIR? 5 : 3;
			if(_operations.PORTA_OUT)
				bufferIndex+=_operations.PORTB_DIR? 5 : 3;

			if(_operations.PORTC_DIR && !_operations.PORTB_OUT)
				bufferIndex+=3;
			if(_operations.PORTB_DIR && !_operations.PORTA_OUT)
				bufferIndex+=3;
			if(_operations.PORTA_DIR)
				bufferIndex+=3;

			bufferIndex+=2;
			_newRegisters.PORTC_IN = inOutBuffer[bufferIndex];
			bufferIndex+=2;
			_newRegisters.PORTB_IN = inOutBuffer[bufferIndex];
			bufferIndex+=2;
			_newRegisters.PORTA_IN = inOutBuffer[bufferIndex];
		}
		DigitalService_ATTiny427Expander_Registers updateRegisters = _newRegisters;
		_operations.First = false;
		_operations.PORTC_OUT = updateRegisters.PORTC_OUT != _previousRegisters.PORTC_OUT;
		_operations.PORTB_OUT = updateRegisters.PORTB_OUT != _previousRegisters.PORTB_OUT;
		_operations.PORTA_OUT = updateRegisters.PORTA_OUT != _previousRegisters.PORTA_OUT;
		_operations.PORTC_DIR = updateRegisters.PORTC_DIR != _previousRegisters.PORTC_DIR;
		_operations.PORTB_DIR = updateRegisters.PORTB_DIR != _previousRegisters.PORTB_DIR;
		_operations.PORTA_DIR = updateRegisters.PORTA_DIR != _previousRegisters.PORTA_DIR;
		
		bufferIndex = 0;
		if(_operations.PORTC_OUT)
		{
			inOutBuffer[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x09; //address
			inOutBuffer[bufferIndex++] = updateRegisters.PORTC_OUT; //OUT
		}
		if(_operations.PORTB_OUT)
		{
			inOutBuffer[bufferIndex++] = _operations.PORTC_DIR? 0xE3 : 0xE1; //write 1 or 3 bytes to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x05; //address
			inOutBuffer[bufferIndex++] = updateRegisters.PORTB_OUT; //OUT
			if(_operations.PORTC_DIR)
			{
				inOutBuffer[bufferIndex++] = 0; //INTFlags. writing 0 to this does nothing
				inOutBuffer[bufferIndex++] = updateRegisters.PORTC_DIR; //DIR
			}
		}
		if(_operations.PORTA_OUT)
		{
			inOutBuffer[bufferIndex++] = _operations.PORTB_DIR? 0xE3 : 0xE1; //write 1 or 3 bytes to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x01; //address
			inOutBuffer[bufferIndex++] = updateRegisters.PORTA_OUT; //OUT
			if(_operations.PORTB_DIR)
			{
				inOutBuffer[bufferIndex++] = 0; //INTFlags. writing 0 to this does nothing
				inOutBuffer[bufferIndex++] = updateRegisters.PORTB_DIR; //DIR
			}
		}

		if(_operations.PORTC_DIR && !_operations.PORTB_OUT)
		{
			inOutBuffer[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x08; //address
			inOutBuffer[bufferIndex++] = updateRegisters.PORTC_DIR; //DIR

		}
		if(_operations.PORTB_DIR && !_operations.PORTA_OUT)
		{
			inOutBuffer[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x04; //address
			inOutBuffer[bufferIndex++] = updateRegisters.PORTB_DIR; //DIR
		}
		if(_operations.PORTA_DIR)
		{
			inOutBuffer[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x00; //address
			inOutBuffer[bufferIndex++] = updateRegisters.PORTA_DIR; //DIR
		}
		inOutBuffer[bufferIndex++] = 0x61; //read 1 byte from 8 bit address with 0 as high address byte
		inOutBuffer[bufferIndex++] = 0x0A; //address
		inOutBuffer[bufferIndex++] = 0x61; //read 1 byte from 8 bit address with 0 as high address byte
		inOutBuffer[bufferIndex++] = 0x06; //address
		inOutBuffer[bufferIndex++] = 0x61; //read 1 byte from 8 bit address with 0 as high address byte
		inOutBuffer[bufferIndex++] = 0x02; //address

		_previousRegisters.PORTA_IN ^= updateRegisters.PORTA_IN;
		_previousRegisters.PORTB_IN ^= updateRegisters.PORTB_IN;
		_previousRegisters.PORTC_IN ^= updateRegisters.PORTC_IN;

		for (DigitalInterruptList_ATTiny427Expander::iterator interrupt = InterruptList.begin(); interrupt != InterruptList.end(); ++interrupt)
		{
			const uint8_t DigitalPin = PinToDigitalPin(interrupt->DigitalPin);
			switch(interrupt->DigitalPort)
			{
				case PORTA: 
						if(_previousRegisters.PORTA_IN & DigitalPin)
							interrupt->CallBack();
					break;
				case PORTB: 
						if(_previousRegisters.PORTB_IN & DigitalPin)
							interrupt->CallBack();
					break;
				case PORTC: 
						if(_previousRegisters.PORTC_IN & DigitalPin)
							interrupt->CallBack();
					break;
			}
		}

		_previousRegisters = updateRegisters;
		return bufferIndex;
	}
}
#endif

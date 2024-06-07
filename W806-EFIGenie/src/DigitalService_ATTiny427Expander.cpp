#include "DigitalService_ATTiny427Expander.h"

#ifdef DIGITALSERVICE_ATTINY427EXPANDER_H
namespace EmbeddedIOServices
{
    DigitalService_ATTiny427Expander::DigitalService_ATTiny427Expander()
    {
    }
	void DigitalService_ATTiny427Expander::InitPin(digitalpin_t pin, PinDirection direction)
	{
		const GPIOPort_ATTiny427Expander GPIOPort = PinToGPIOPort(pin);
		const GPIOPin_ATTiny427Expander GPIOPin = PinToGPIOPin(pin);
		switch(GPIOPort)
		{
			case PORTA:
				if(direction == In)
					_newRegisters.PORTA_DIR &= ~GPIOPin;
				else
					_newRegisters.PORTA_DIR |= GPIOPin;
				return;
			case PORTB:
				if(direction == In)
					_newRegisters.PORTB_DIR &= ~GPIOPin;
				else
					_newRegisters.PORTB_DIR |= GPIOPin;
				return;
			case PORTC:
				if(direction == In)
					_newRegisters.PORTC_DIR &= ~GPIOPin;
				else
					_newRegisters.PORTC_DIR |= GPIOPin;
				return;
		}
	}
	bool DigitalService_ATTiny427Expander::ReadPin(digitalpin_t pin)
	{
		const GPIOPort_ATTiny427Expander GPIOPort = PinToGPIOPort(pin);
		const GPIOPin_ATTiny427Expander GPIOPin = PinToGPIOPin(pin);
		switch(GPIOPort)
		{
			case PORTA:
				return _newRegisters.PORTA_IN & GPIOPin;
			case PORTB:
				return _newRegisters.PORTB_IN & GPIOPin;
			case PORTC:
				return _newRegisters.PORTC_IN & GPIOPin;
		}
	}
	void DigitalService_ATTiny427Expander::WritePin(digitalpin_t pin, bool value)
	{
		const GPIOPort_ATTiny427Expander GPIOPort = PinToGPIOPort(pin);
		const GPIOPin_ATTiny427Expander GPIOPin = PinToGPIOPin(pin);
		switch(pin>>3)
		{
			case PORTA:
				if(value == false)
					_newRegisters.PORTA_OUT &= ~GPIOPin;
				else
					_newRegisters.PORTA_OUT |= GPIOPin;
				return;
			case PORTB:
				if(value == false)
					_newRegisters.PORTB_OUT &= ~GPIOPin;
				else
					_newRegisters.PORTB_OUT |= GPIOPin;
				return;
			case PORTC:
				if(value == false)
					_newRegisters.PORTC_OUT &= ~GPIOPin;
				else
					_newRegisters.PORTC_OUT |= GPIOPin;
				return;
		}
	}
	void DigitalService_ATTiny427Expander::AttachInterrupt(digitalpin_t pin, callback_t callBack)
	{
		const GPIOPort_ATTiny427Expander GPIOPort = PinToGPIOPort(pin);
		const GPIOPin_ATTiny427Expander GPIOPin = PinToGPIOPin(pin);
		InterruptList.push_front(DigitalInterrupt_ATTiny427Expander(GPIOPort, GPIOPin, callBack));

	}
	void DigitalService_ATTiny427Expander::DetachInterrupt(digitalpin_t pin)
	{
		const GPIOPort_ATTiny427Expander GPIOPort = PinToGPIOPort(pin);
		const GPIOPin_ATTiny427Expander GPIOPin = PinToGPIOPin(pin);
		InterruptList.remove_if([GPIOPort, GPIOPin](const DigitalInterrupt_ATTiny427Expander& interrupt) { return interrupt.GPIOPort == GPIOPort && interrupt.GPIOPin == GPIOPin; });
	}
	size_t DigitalService_ATTiny427Expander::Update(uint8_t inOutBuffer[88])
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
		bufferIndex = 0;
		_operations.First = false;
		_operations.PORTC_OUT = _newRegisters.PORTC_OUT != _previousRegisters.PORTC_OUT;
		_operations.PORTB_OUT = _newRegisters.PORTB_OUT != _previousRegisters.PORTB_OUT;
		_operations.PORTA_OUT = _newRegisters.PORTA_OUT != _previousRegisters.PORTA_OUT;
		_operations.PORTC_DIR = _newRegisters.PORTC_DIR != _previousRegisters.PORTC_DIR;
		_operations.PORTB_DIR = _newRegisters.PORTB_DIR != _previousRegisters.PORTB_DIR;
		_operations.PORTA_DIR = _newRegisters.PORTA_DIR != _previousRegisters.PORTA_DIR;
		
		
		if(_operations.PORTC_OUT)
		{
			inOutBuffer[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x09; //address
			inOutBuffer[bufferIndex++] = _newRegisters.PORTC_OUT; //OUT
		}
		if(_operations.PORTB_OUT)
		{
			inOutBuffer[bufferIndex++] = _operations.PORTC_DIR? 0xE3 : 0xE1; //write 1 or 3 bytes to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x05; //address
			inOutBuffer[bufferIndex++] = _newRegisters.PORTB_OUT; //OUT
			if(_operations.PORTC_DIR)
			{
				inOutBuffer[bufferIndex++] = 0; //INTFlags. writing 0 to this does nothing
				inOutBuffer[bufferIndex++] = _newRegisters.PORTC_DIR; //DIR
			}
		}
		if(_operations.PORTA_OUT)
		{
			inOutBuffer[bufferIndex++] = _operations.PORTB_DIR? 0xE3 : 0xE1; //write 1 or 3 bytes to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x01; //address
			inOutBuffer[bufferIndex++] = _newRegisters.PORTA_OUT; //OUT
			if(_operations.PORTB_DIR)
			{
				inOutBuffer[bufferIndex++] = 0; //INTFlags. writing 0 to this does nothing
				inOutBuffer[bufferIndex++] = _newRegisters.PORTB_DIR; //DIR
			}
		}

		if(_operations.PORTC_DIR && !_operations.PORTB_OUT)
		{
			inOutBuffer[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x08; //address
			inOutBuffer[bufferIndex++] = _newRegisters.PORTC_DIR; //DIR

		}
		if(_operations.PORTB_DIR && !_operations.PORTA_OUT)
		{
			inOutBuffer[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x04; //address
			inOutBuffer[bufferIndex++] = _newRegisters.PORTB_DIR; //DIR
		}
		if(_operations.PORTA_DIR)
		{
			inOutBuffer[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			inOutBuffer[bufferIndex++] = 0x00; //address
			inOutBuffer[bufferIndex++] = _newRegisters.PORTA_DIR; //DIR
		}
		inOutBuffer[bufferIndex++] = 0x61; //read 1 byte from 8 bit address with 0 as high address byte
		inOutBuffer[bufferIndex++] = 0x0A; //address
		inOutBuffer[bufferIndex++] = 0x61; //read 1 byte from 8 bit address with 0 as high address byte
		inOutBuffer[bufferIndex++] = 0x06; //address
		inOutBuffer[bufferIndex++] = 0x61; //read 1 byte from 8 bit address with 0 as high address byte
		inOutBuffer[bufferIndex++] = 0x02; //address

		_previousRegisters.PORTA_IN ^= _newRegisters.PORTA_IN;
		_previousRegisters.PORTB_IN ^= _newRegisters.PORTB_IN;
		_previousRegisters.PORTC_IN ^= _newRegisters.PORTC_IN;

		for (DigitalInterruptList_ATTiny427Expander::iterator interrupt = InterruptList.begin(); interrupt != InterruptList.end(); ++interrupt)
		{
			const uint8_t GPIOPin = PinToGPIOPin(interrupt->GPIOPin);
			switch(interrupt->GPIOPort)
			{
				case PORTA: 
						if(_previousRegisters.PORTA_IN & GPIOPin)
							interrupt->CallBack();
					break;
				case PORTB: 
						if(_previousRegisters.PORTB_IN & GPIOPin)
							interrupt->CallBack();
					break;
				case PORTC: 
						if(_previousRegisters.PORTC_IN & GPIOPin)
							interrupt->CallBack();
					break;
			}
		}
		
		_previousRegisters = _newRegisters;
		return bufferIndex;
	}
}
#endif

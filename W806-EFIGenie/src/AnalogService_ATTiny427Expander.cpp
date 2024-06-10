#include "AnalogService_ATTiny427Expander.h"

#ifdef ANALOGSERVICE_ATTINY427EXPANDER_H
namespace EmbeddedIOServices
{
	void AnalogService_ATTiny427Expander::InitPin(analogpin_t pin)
	{
        const AnalogChannel_ATTiny427Expander AnalogChannel = PinToAnalogChannel(pin);
        if(AnalogChannel > 7)
            _newRegisters.AnalogEnable1 |= 0x1 << (AnalogChannel - 8);
        else
            _newRegisters.AnalogEnable0 |= 0x1 << AnalogChannel;
	}
	
	float AnalogService_ATTiny427Expander::ReadPin(analogpin_t pin)
	{
        const AnalogChannel_ATTiny427Expander AnalogChannel = PinToAnalogChannel(pin);
        return _value[AnalogChannel] * (5.0f / (2^16 - 1));
	}

	size_t AnalogService_ATTiny427Expander::Update(uint8_t inOutBuffer[30])
	{
		uint8_t bufferIndex = 0;
		if(!_operations.First) 
		{
			bufferIndex+=3;
            _newRegisters.Counter = inOutBuffer[bufferIndex++];
            if(_previousRegisters.AnalogEnable0 & 0b00000010)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[1] = value;
            }
            if(_previousRegisters.AnalogEnable0 & 0b00000100)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[2] = value;
            }
            if(_previousRegisters.AnalogEnable0 & 0b00001000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[3] = value;
            }
            if(_previousRegisters.AnalogEnable0 & 0b00010000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[4] = value;
            }
            if(_previousRegisters.AnalogEnable0 & 0b00100000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[5] = value;
            }
            if(_previousRegisters.AnalogEnable0 & 0b01000000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[6] = value;
            }
            if(_previousRegisters.AnalogEnable0 & 0b10000000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[7] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b00000001)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[8] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b00000010)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[9] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b00000100)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[10] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b00001000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[11] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b00010000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[12] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b00100000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[13] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b01000000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[14] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b10000000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[15] = value;
            }

			if(_operations.AnalogEnable0)
				bufferIndex+=5;
            else if(_operations.AnalogEnable1)
				bufferIndex+=4;
            else if(_operations.AnalogAccumulate)
				bufferIndex+=3;
		}
		AnalogService_ATTiny427Expander_Registers updateRegisters = _newRegisters;
        _operations.AnalogEnable0 = updateRegisters.AnalogEnable0 != _previousRegisters.AnalogEnable0;
        _operations.AnalogEnable1 = updateRegisters.AnalogEnable1 != _previousRegisters.AnalogEnable1;
        _operations.AnalogAccumulate = updateRegisters.AnalogAccumulate != _previousRegisters.AnalogAccumulate;

        bufferIndex=0;
        uint8_t length = 0;
        if(updateRegisters.AnalogEnable0 & 0b00000010)
            length++;
        if(updateRegisters.AnalogEnable0 & 0b00000100)
            length++;
        if(updateRegisters.AnalogEnable0 & 0b00001000)
            length++;
        if(updateRegisters.AnalogEnable0 & 0b00010000)
            length++;
        if(updateRegisters.AnalogEnable0 & 0b00100000)
            length++;
        if(updateRegisters.AnalogEnable0 & 0b01000000)
            length++;
        if(updateRegisters.AnalogEnable0 & 0b10000000)
            length++;
        if(updateRegisters.AnalogEnable1 & 0b00000001)
            length++;
        if(updateRegisters.AnalogEnable1 & 0b00000010)
            length++;
        if(updateRegisters.AnalogEnable1 & 0b00000100)
            length++;
        if(updateRegisters.AnalogEnable1 & 0b00001000)
            length++;
        if(updateRegisters.AnalogEnable1 & 0b00010000)
            length++;
        if(updateRegisters.AnalogEnable1 & 0b00100000)
            length++;
        if(updateRegisters.AnalogEnable1 & 0b01000000)
            length++;
        if(updateRegisters.AnalogEnable1 & 0b10000000)
            length++;
        length *= 2;
        length += 1;
        inOutBuffer[bufferIndex++] = length;
        inOutBuffer[bufferIndex++] = 0x34;
        inOutBuffer[bufferIndex++] = 0x0D;
        for(uint8_t i; i<length-1; i++)
            inOutBuffer[bufferIndex++] = 0;

        if(_operations.AnalogEnable0)
        {
            inOutBuffer[bufferIndex++] = 0x83;
            inOutBuffer[bufferIndex++] = 0x1C;
            inOutBuffer[bufferIndex++] = updateRegisters.AnalogEnable0;
            inOutBuffer[bufferIndex++] = updateRegisters.AnalogEnable1;
            inOutBuffer[bufferIndex++] = updateRegisters.AnalogAccumulate | 0x80;
        }
        else if(_operations.AnalogEnable1)
        {
            inOutBuffer[bufferIndex++] = 0x82;
            inOutBuffer[bufferIndex++] = 0x1D;
            inOutBuffer[bufferIndex++] = updateRegisters.AnalogEnable1;
            inOutBuffer[bufferIndex++] = updateRegisters.AnalogAccumulate | 0x80;
        }
        else if(_operations.AnalogAccumulate)
        {
            inOutBuffer[bufferIndex++] = 0x81;
            inOutBuffer[bufferIndex++] = 0x1E;
            inOutBuffer[bufferIndex++] = updateRegisters.AnalogAccumulate | 0x80;
        }

        _previousRegisters = updateRegisters;

        //TODO initial ADC setup

        return bufferIndex;
    }
}
#endif

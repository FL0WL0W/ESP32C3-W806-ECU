#include "AnalogService_ATTiny427Expander.h"

#ifdef ANALOGSERVICE_ATTINY427EXPANDER_H
namespace EmbeddedIOServices
{
	AnalogService_ATTiny427Expander::AnalogService_ATTiny427Expander(const bool altSPI) : _altSPI(altSPI)
    {

    }

	void AnalogService_ATTiny427Expander::InitPin(analogpin_t pin)
	{
        const AnalogChannel_ATTiny427Expander AnalogChannel = PinToAnalogChannel(pin);
        if(AnalogChannel > 7)
        {
            if(AnalogChannel < 5 && !_altSPI)
                return;
            _newRegisters.AnalogEnable1 |= 0x1 << (AnalogChannel - 8);
        }
        else
        {
            if(AnalogChannel >11 && _altSPI)
                return;
            _newRegisters.AnalogEnable0 |= 0x1 << AnalogChannel;
        }
	}
	
	float AnalogService_ATTiny427Expander::ReadPin(analogpin_t pin)
	{
        const AnalogChannel_ATTiny427Expander AnalogChannel = PinToAnalogChannel(pin);
        return _value[AnalogChannel - (!_altSPI? 5 : 0)] * (5.0f / (2^16 - 1));
	}

	size_t AnalogService_ATTiny427Expander::Update(uint8_t inOutBuffer[43])
	{
		uint8_t bufferIndex = 0;
		if(!_operations.First) 
		{
			bufferIndex+=3;
            _newRegisters.Counter = inOutBuffer[bufferIndex++];
            if(_altSPI)
            {
                if(_previousRegisters.AnalogEnable0 & 0b00000010)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _value[0] = value;
                }
                if(_previousRegisters.AnalogEnable0 & 0b00000100)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _value[1] = value;
                }
                if(_previousRegisters.AnalogEnable0 & 0b00001000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _value[2] = value;
                }
                if(_previousRegisters.AnalogEnable0 & 0b00010000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _value[3] = value;
                }
            }
            if(_previousRegisters.AnalogEnable0 & 0b00100000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[(!_altSPI? 4 : 0) + 0] = value;
            }
            if(_previousRegisters.AnalogEnable0 & 0b01000000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[(!_altSPI? 4 : 0) + 1] = value;
            }
            if(_previousRegisters.AnalogEnable0 & 0b10000000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[(!_altSPI? 4 : 0) + 2] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b00000001)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[(!_altSPI? 4 : 0) + 3] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b00000010)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[(!_altSPI? 4 : 0) + 4] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b00000100)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[(!_altSPI? 4 : 0) + 5] = value;
            }
            if(_previousRegisters.AnalogEnable1 & 0b00001000)
            {
                uint16_t value = inOutBuffer[bufferIndex++];
                value |= inOutBuffer[bufferIndex++] << 8;
                _value[(!_altSPI? 4 : 0) + 6] = value;
            }
            if(!_altSPI)
            {
                if(_previousRegisters.AnalogEnable1 & 0b00010000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _value[7] = value;
                }
                if(_previousRegisters.AnalogEnable1 & 0b00100000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _value[8] = value;
                }
                if(_previousRegisters.AnalogEnable1 & 0b01000000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _value[9] = value;
                }
                if(_previousRegisters.AnalogEnable1 & 0b10000000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _value[10] = value;
                }
            }

			if(_operations.AnalogEnable0)
				bufferIndex+=5;
            else if(_operations.AnalogEnable1)
				bufferIndex+=4;
            else if(_operations.AnalogAccumulate)
				bufferIndex+=3;
		}
		AnalogService_ATTiny427Expander_Registers updateRegisters = _newRegisters;

        bufferIndex=0;
        uint8_t length = 0;
        if(_altSPI)
        {
            if(updateRegisters.AnalogEnable0 & 0b00000010)
                length++;
            if(updateRegisters.AnalogEnable0 & 0b00000100)
                length++;
            if(updateRegisters.AnalogEnable0 & 0b00001000)
                length++;
            if(updateRegisters.AnalogEnable0 & 0b00010000)
                length++;
        }
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
        if(!_altSPI)
        {
            if(updateRegisters.AnalogEnable1 & 0b00010000)
                length++;
            if(updateRegisters.AnalogEnable1 & 0b00100000)
                length++;
            if(updateRegisters.AnalogEnable1 & 0b01000000)
                length++;
            if(updateRegisters.AnalogEnable1 & 0b10000000)
                length++;
        }
        length *= 2;
        length += 1;
        if(length > 1)
        {
            _operations.AnalogEnable0 = updateRegisters.AnalogEnable0 != _previousRegisters.AnalogEnable0 || _operations.First;
            _operations.AnalogEnable1 = updateRegisters.AnalogEnable1 != _previousRegisters.AnalogEnable1 || _operations.First;
            _operations.AnalogAccumulate = updateRegisters.AnalogAccumulate != _previousRegisters.AnalogAccumulate || _operations.First;

            inOutBuffer[bufferIndex++] = length;
            inOutBuffer[bufferIndex++] = 0x34;
            inOutBuffer[bufferIndex++] = 0x0D;
            for(uint8_t i; i<length-1; i++)
                inOutBuffer[bufferIndex++] = 0;

            if(_operations.AnalogEnable0)
            {
                inOutBuffer[bufferIndex++] = 0xE3;
                inOutBuffer[bufferIndex++] = 0x1C;
                inOutBuffer[bufferIndex++] = updateRegisters.AnalogEnable0;
                inOutBuffer[bufferIndex++] = updateRegisters.AnalogEnable1;
                inOutBuffer[bufferIndex++] = updateRegisters.AnalogAccumulate | 0x80;
            }
            else if(_operations.AnalogEnable1)
            {
                inOutBuffer[bufferIndex++] = 0xE2;
                inOutBuffer[bufferIndex++] = 0x1D;
                inOutBuffer[bufferIndex++] = updateRegisters.AnalogEnable1;
                inOutBuffer[bufferIndex++] = updateRegisters.AnalogAccumulate | 0x80;
            }
            else if(_operations.AnalogAccumulate)
            {
                inOutBuffer[bufferIndex++] = 0xE1;
                inOutBuffer[bufferIndex++] = 0x1E;
                inOutBuffer[bufferIndex++] = updateRegisters.AnalogAccumulate | 0x80;
            }

            //TODO initial ADC setup
            if(_operations.First)
            {
                inOutBuffer[bufferIndex++] = 0x85; //write 16bit address
                inOutBuffer[bufferIndex++] = 0x06; //address high
                inOutBuffer[bufferIndex++] = 0x00; //address low
                inOutBuffer[bufferIndex++] = 0b00100001; //enable ADC with low latency
                inOutBuffer[bufferIndex++] = 0x01; //prescaler DIV4 to get ADCCLK 5MHZ
                inOutBuffer[bufferIndex++] = 0xA0; //set timebase and VDD reference
                inOutBuffer[bufferIndex++] = 0x00; //no window source mode
                inOutBuffer[bufferIndex++] = 0x01; //enable RESRDY interrupt
                inOutBuffer[bufferIndex++] = 0xC3; //write 3 bytes to 8 bit address using existing high byte
                inOutBuffer[bufferIndex++] = 0x08; //address low
                inOutBuffer[bufferIndex++] = 0x05; //SAMPDUR = 5
                inOutBuffer[bufferIndex++] = 0x00; //co accumulation. accumulation done in software so the readings are evenly spaced
                inOutBuffer[bufferIndex++] = 0x11; //single 12 bit mode and start
            }
        }
        else
        {
            _operations.AnalogEnable0 = false;
            _operations.AnalogEnable1 = false;
            _operations.AnalogAccumulate = false;
        }


        _previousRegisters = updateRegisters;
        return bufferIndex;
    }
}
#endif

#include "ATTiny427ExpanderUpdateService.h"

#ifdef ATTINY427EXPANDERUPDATESERVICE_H
namespace EmbeddedIOServices
{
    ATTiny427ExpanderUpdateService::ATTiny427ExpanderUpdateService(ATTiny427Expander_Registers *registers) : _registers(registers)
    {
		_previousRegisters = *_registers; 
    }
    void ATTiny427ExpanderUpdateService::Receive(uint8_t *data, size_t size)
    {
        size_t dataIndex = 0;
        while(dataIndex < size)
        {
            switch(_receiveState)
            {
                case 0: // waiting for read command
                    break;
                case 1: // reading high address
                    break;
                case 2: // reading low address
                    switch(_receiveAddress)
                    {
                        case 0x0000:
                            _receiveState = 3;
                            _receiveIndex = 0;
                            break;
                        case 0x3400:
                            _receiveState = 4;
                            _receiveIndex = 0;
                            break;
                        case 0x3405:
                            _receiveState = 5;
                            _receiveLength = 5;
                            if(_analogReceiveLength > 0)
                                _receiveLength += _analogReceiveLength + 4;
                        case 0x340A:
                            _receiveState = 6;
                            _receiveLength = 3;

                    }
                    break;
                case 3: // reading GPIO
                    break;
                case 4: // reading TCB0
                case 5: // reading TCB1
                case 6: // reading gpio from main loop
                case 7: // reading analog
                    break;
            }

            dataIndex++;
        }
    }

	size_t ATTiny427ExpanderUpdateService::Update(uint8_t inOutBuffer[1024])
	{
		uint8_t bufferIndex = 0;
		if(!_operations.First) //read
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
			_registers->PORTC_IN = inOutBuffer[bufferIndex];
			bufferIndex+=2;
			_registers->PORTB_IN = inOutBuffer[bufferIndex];
			bufferIndex+=2;
			_registers->PORTA_IN = inOutBuffer[bufferIndex];

			bufferIndex+=3;

            if(_operations.AnalogRead != 0)
            {
                _registers->AnalogCounter = inOutBuffer[bufferIndex++];
                if(_registers->AlternateSPI)
                {
                    if(_operations.AnalogRead & 0b0000000000000010)
                    {
                        uint16_t value = inOutBuffer[bufferIndex++];
                        value |= inOutBuffer[bufferIndex++] << 8;
                        _registers->AnalogValues[0] = value;
                    }
                    if(_operations.AnalogRead & 0b0000000000000100)
                    {
                        uint16_t value = inOutBuffer[bufferIndex++];
                        value |= inOutBuffer[bufferIndex++] << 8;
                        _registers->AnalogValues[1] = value;
                    }
                    if(_operations.AnalogRead & 0b0000000000001000)
                    {
                        uint16_t value = inOutBuffer[bufferIndex++];
                        value |= inOutBuffer[bufferIndex++] << 8;
                        _registers->AnalogValues[2] = value;
                    }
                    if(_operations.AnalogRead & 0b0000000000010000)
                    {
                        uint16_t value = inOutBuffer[bufferIndex++];
                        value |= inOutBuffer[bufferIndex++] << 8;
                        _registers->AnalogValues[3] = value;
                    }
                }
                if(_operations.AnalogRead & 0b0000000000100000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _registers->AnalogValues[(!_registers->AlternateSPI? 4 : 0) + 0] = value;
                }
                if(_operations.AnalogRead & 0b0000000001000000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _registers->AnalogValues[(!_registers->AlternateSPI? 4 : 0) + 1] = value;
                }
                if(_operations.AnalogRead & 0b0000000010000000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _registers->AnalogValues[(!_registers->AlternateSPI? 4 : 0) + 2] = value;
                }
                if(_operations.AnalogRead & 0b0000000100000000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _registers->AnalogValues[(!_registers->AlternateSPI? 4 : 0) + 3] = value;
                }
                if(_operations.AnalogRead & 0b0000001000000000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _registers->AnalogValues[(!_registers->AlternateSPI? 4 : 0) + 4] = value;
                }
                if(_operations.AnalogRead & 0b0000010000000000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _registers->AnalogValues[(!_registers->AlternateSPI? 4 : 0) + 5] = value;
                }
                if(_operations.AnalogRead & 0b0000100000000000)
                {
                    uint16_t value = inOutBuffer[bufferIndex++];
                    value |= inOutBuffer[bufferIndex++] << 8;
                    _registers->AnalogValues[(!_registers->AlternateSPI? 4 : 0) + 6] = value;
                }
                if(!_registers->AlternateSPI)
                {
                    if(_operations.AnalogRead & 0b0001000000000000)
                    {
                        uint16_t value = inOutBuffer[bufferIndex++];
                        value |= inOutBuffer[bufferIndex++] << 8;
                        _registers->AnalogValues[7] = value;
                    }
                    if(_operations.AnalogRead & 0b0010000000000000)
                    {
                        uint16_t value = inOutBuffer[bufferIndex++];
                        value |= inOutBuffer[bufferIndex++] << 8;
                        _registers->AnalogValues[8] = value;
                    }
                    if(_operations.AnalogRead & 0b0100000000000000)
                    {
                        uint16_t value = inOutBuffer[bufferIndex++];
                        value |= inOutBuffer[bufferIndex++] << 8;
                        _registers->AnalogValues[9] = value;
                    }
                    if(_operations.AnalogRead & 0b1000000000000000)
                    {
                        uint16_t value = inOutBuffer[bufferIndex++];
                        value |= inOutBuffer[bufferIndex++] << 8;
                        _registers->AnalogValues[10] = value;
                    }
                }
            }

            if(_operations.AnalogStop)
                bufferIndex+=0;
            if(_operations.GPIOR0)
                bufferIndex+=5;
            else if(_operations.GPIOR1)
                bufferIndex+=4;
            else if(_operations.GPIOR2)
                bufferIndex+=3;
            if(_operations.AnalogStart)
                bufferIndex+=13;

		    bufferIndex = 0;
		}

		ATTiny427Expander_Registers updateRegisters = *_registers;

        //Digital
        if(_registers->AlternateSPI)
        {
            updateRegisters.PORTC_DIR |= 0b00000010;
            updateRegisters.PORTC_DIR &= 0b11110000;
        }
        else
        {
            updateRegisters.PORTA_DIR |= 0b00000100;
            updateRegisters.PORTA_DIR &= 0b11100000;
        }
		_operations.PORTC_OUT = updateRegisters.PORTC_OUT != _previousRegisters.PORTC_OUT || _operations.First;
		_operations.PORTB_OUT = updateRegisters.PORTB_OUT != _previousRegisters.PORTB_OUT || _operations.First;
		_operations.PORTA_OUT = updateRegisters.PORTA_OUT != _previousRegisters.PORTA_OUT || _operations.First;
		_operations.PORTC_DIR = updateRegisters.PORTC_DIR != _previousRegisters.PORTC_DIR || _operations.First;
		_operations.PORTB_DIR = updateRegisters.PORTB_DIR != _previousRegisters.PORTB_DIR || _operations.First;
		_operations.PORTA_DIR = updateRegisters.PORTA_DIR != _previousRegisters.PORTA_DIR || _operations.First;
		
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

        //Analog
        if(_registers->AlternateSPI)
        {
            updateRegisters.AnalogEnable1 &= 0x0F;
            updateRegisters.AnalogEnable0 &= 0xFE;
        }
        else
        {
            updateRegisters.AnalogEnable0 &= 0xE0;
        }
        _operations.AnalogRead = (_previousRegisters.AnalogEnable1 << 8) | _previousRegisters.AnalogEnable0;
        _operations.AnalogStop = ((updateRegisters.AnalogEnable0 == 0 || updateRegisters.AnalogEnable1 == 0) && _operations.AnalogRunning) || _operations.First;
        _operations.GPIOR0 = updateRegisters.GPIOR0 != _previousRegisters.GPIOR0 || _operations.First;
        _operations.GPIOR1 = updateRegisters.GPIOR1 != _previousRegisters.GPIOR1 || _operations.First;
        _operations.GPIOR2 = updateRegisters.GPIOR2 != _previousRegisters.GPIOR2 || _operations.First;
        _operations.AnalogStart = updateRegisters.AnalogEnable0 != 0 && updateRegisters.AnalogEnable1 != 0 && !_operations.AnalogRunning;
        
        if(_operations.AnalogRead != 0)
        {
            uint8_t analogChannelCount = 0;
            if(_registers->AlternateSPI)
            {
                if(_operations.AnalogRead & 0b0000000000000010)
                    analogChannelCount++;
                if(_operations.AnalogRead & 0b0000000000000100)
                    analogChannelCount++;
                if(_operations.AnalogRead & 0b0000000000001000)
                    analogChannelCount++;
                if(_operations.AnalogRead & 0b0000000000010000)
                    analogChannelCount++;
            }
            else
            {
                if(_operations.AnalogRead & 0b0001000000000000)
                    analogChannelCount++;
                if(_operations.AnalogRead & 0b0010000000000000)
                    analogChannelCount++;
                if(_operations.AnalogRead & 0b0100000000000000)
                    analogChannelCount++;
                if(_operations.AnalogRead & 0b1000000000000000)
                    analogChannelCount++;
            }
            if(_operations.AnalogRead & 0b0000000000100000)
                analogChannelCount++;
            if(_operations.AnalogRead & 0b0000000001000000)
                analogChannelCount++;
            if(_operations.AnalogRead & 0b0000000010000000)
                analogChannelCount++;
            if(_operations.AnalogRead & 0b0000000100000000)
                analogChannelCount++;
            if(_operations.AnalogRead & 0b0000001000000000)
                analogChannelCount++;
            if(_operations.AnalogRead & 0b0000010000000000)
                analogChannelCount++;
            if(_operations.AnalogRead & 0b0000100000000000)
                analogChannelCount++;

            inOutBuffer[bufferIndex++] = analogChannelCount * 2 + 1;
            inOutBuffer[bufferIndex++] = 0x34;
            inOutBuffer[bufferIndex++] = 0x0D;
            for(uint8_t i = 0; i<analogChannelCount; i++)
            {
                inOutBuffer[bufferIndex++] = 0;
                inOutBuffer[bufferIndex++] = 0;
            }
        }

        if(_operations.AnalogStop)
        {
            //todo stop analog
        }

        if(_operations.GPIOR0)
        {
            inOutBuffer[bufferIndex++] = 0xE3;
            inOutBuffer[bufferIndex++] = 0x1C;
            inOutBuffer[bufferIndex++] = updateRegisters.GPIOR0;
            inOutBuffer[bufferIndex++] = updateRegisters.GPIOR1;
            inOutBuffer[bufferIndex++] = updateRegisters.GPIOR2 | 0x80;
        }
        else if(_operations.GPIOR1)
        {
            inOutBuffer[bufferIndex++] = 0xE2;
            inOutBuffer[bufferIndex++] = 0x1D;
            inOutBuffer[bufferIndex++] = updateRegisters.GPIOR1;
            inOutBuffer[bufferIndex++] = updateRegisters.GPIOR2 | 0x80;
        }
        else if(_operations.GPIOR2)
        {
            inOutBuffer[bufferIndex++] = 0xE1;
            inOutBuffer[bufferIndex++] = 0x1E;
            inOutBuffer[bufferIndex++] = updateRegisters.GPIOR2 | (updateRegisters.AnalogAccumulate != _previousRegisters.AnalogAccumulate? 0x80 : 0x00);
        }

        if(_operations.AnalogStart)
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
            inOutBuffer[bufferIndex++] = 0x05; //SAMPDUR = 5. TODO add this as an adjustable parameter from analogservice
            inOutBuffer[bufferIndex++] = 0x00; //co accumulation. accumulation done in software so the readings are evenly spaced
            inOutBuffer[bufferIndex++] = 0x11; //single 12 bit mode and start
            _operations.AnalogRunning = true;
        }

        //AC
        _operations.AC_CTRLA = updateRegisters.AC_CTRLA != _previousRegisters.AC_CTRLA || _operations.First;
        _operations.AC_MUXCTRLA = updateRegisters.AC_MUXCTRLA != _previousRegisters.AC_MUXCTRLA || _operations.First;
        _operations.AC_DACREF = updateRegisters.AC_DACREF != _previousRegisters.AC_DACREF || _operations.First;

        if(_operations.AC_CTRLA)
        {
            inOutBuffer[bufferIndex++] = 0x81 + (_operations.AC_MUXCTRLA && _operations.AC_DACREF? 4 : (_operations.AC_MUXCTRLA? 2 : 0)); //write 16bit address
            inOutBuffer[bufferIndex++] = 0x06; //address high
            inOutBuffer[bufferIndex++] = 0x80; //address low
            inOutBuffer[bufferIndex++] = updateRegisters.AC_CTRLA; //CTRLA
            if(_operations.AC_MUXCTRLA)
            {
                inOutBuffer[bufferIndex++] = 0; //nothing byte
                inOutBuffer[bufferIndex++] = updateRegisters.AC_MUXCTRLA; //MUXCTRLA
                if(_operations.AC_DACREF)
                {
                    inOutBuffer[bufferIndex++] = 0; //nothing byte
                    inOutBuffer[bufferIndex++] = updateRegisters.AC_DACREF; //DACREF
                }
            }
            else if(_operations.AC_DACREF)
            {
                inOutBuffer[bufferIndex++] = 0xC1; //write 1 byte to 8 bit address using existing high byte
                inOutBuffer[bufferIndex++] = 0x82; //low byte
                inOutBuffer[bufferIndex++] = updateRegisters.AC_DACREF; //DACREF
            }
        }
        else if(_operations.AC_MUXCTRLA)
        {
            inOutBuffer[bufferIndex++] = 0x81 + (_operations.AC_DACREF? 2 : 0); //write 16bit address
            inOutBuffer[bufferIndex++] = 0x06; //address high
            inOutBuffer[bufferIndex++] = 0x82; //address low
            inOutBuffer[bufferIndex++] = updateRegisters.AC_MUXCTRLA; //MUXCTRLA
            if(_operations.AC_DACREF)
            {
                inOutBuffer[bufferIndex++] = 0; //nothing byte
                inOutBuffer[bufferIndex++] = updateRegisters.AC_DACREF; //DACREF
            }
        }
        else if(_operations.AC_DACREF)
        {
            inOutBuffer[bufferIndex++] = 0x81; //write 16bit address
            inOutBuffer[bufferIndex++] = 0x06; //address high
            inOutBuffer[bufferIndex++] = 0x84; //address low
            inOutBuffer[bufferIndex++] = updateRegisters.AC_DACREF; //DACREF
        }

		_operations.First = false;
		_previousRegisters = updateRegisters;
		return bufferIndex;
	}
}
#endif

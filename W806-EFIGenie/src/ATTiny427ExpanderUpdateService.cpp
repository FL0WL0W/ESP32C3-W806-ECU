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
                    if(data[0] & 0b0001000) // no continuation read commands should be echoed, flag as invalid
                        _invalidReceive = true;
                    if(data[0] & 0b1000000) //Write command should not be echoed, flag as invalid
                        _invalidReceive = true;
                    if(data[0] & 0b0100000)
                    {
                        if(!(data[0] & 0b0010000))
                            _invalidReceive = true;
                        _receiveState = 2;
                        _receiveAddress = 0;
                    }
                    else if(data[0] & 0b0010000)
                        _invalidReceive = true;
                    //using length command as analog enable count. it must match what we transmitted or there is a some disconnect with the channel allocations, flag as invalid
                    _analogEnableCount = data[0] & 0x0F - 1;

                    if(_invalidReceive || data[0] & 0x0F == 0) //check if we've received an invalid command and check for 0 length command
                        _receiveState = 0;
                    else
                        _receiveState = 1;
                    break;
                case 1: // reading high address
                    _receiveAddress = data[dataIndex] << 8;
                    _receiveState++;
                    break;
                case 2: // reading low address
                    _receiveAddress |= data[dataIndex];
                    _receiveState++;
                    break;
                case 3: // reading
                    if(_receiveAddress == 0x0002)
                    {
                        _registers->PORTA_IN = data[dataIndex];
                        _receiveState = 2;
                        _receiveAddress = 0;
                    }
                    else if(_receiveAddress == 0x0006)
                    {
                        _registers->PORTB_IN = data[dataIndex];
                        _receiveState = 2;
                        _receiveAddress = 0;
                    }
                    else if(_receiveAddress == 0x000A)
                    {
                        _registers->PORTC_IN = data[dataIndex];
                        _receiveState = 0;
                    }
                    else if(_receiveAddress == 0x3400)
                    {
                        if(_receiveIndex != 0)
                        {
                            //invalid
                            _invalidReceive = true;
                            _receiveState = 0;
                            _receiveIndex = 0;
                        }
                        else
                        {
                            _receiveData[0] = data[dataIndex];
                            _receiveIndex = 1;
                        }
                    }
                    else if(_receiveAddress == 0x3405)
                    {
                        if(_receiveIndex == 5)
                        {
                            //set tcb0 value
                            _registers->TCB0CNT = _receiveData[1] | (_receiveData[2] << 8);
                            _registers->TCB0CCMP = _receiveData[3] | (_receiveData[4] << 8);
                            _registers->TCB0Counter = _receiveData[0];
                            _receiveIndex = 0;
                        }
                        if(_receiveIndex != 0)
                        {
                            //invalid
                            _invalidReceive = true;
                            _receiveState = 0;
                            _receiveIndex = 0;
                        }
                        else
                        {
                            _receiveData[0] = data[dataIndex];
                            _receiveIndex = 1;
                        }
                    }
                    else if(_receiveAddress == 0x340A)
                    {
                        if(_receiveIndex == 5)
                        {
                            //set tcb1 value
                            _registers->TCB1CNT = _receiveData[1] | (_receiveData[2] << 8);
                            _registers->TCB1CCMP = _receiveData[3] | (_receiveData[4] << 8);
                            _registers->TCB1Counter = _receiveData[0];
                            _receiveIndex = 0;
                        }
                        if(_receiveIndex != 0)
                        {
                            //invalid
                            _invalidReceive = true;
                            _receiveState = 0;
                            _receiveIndex = 0;
                        }
                        else
                        {
                            //maybe set GPIOA
                        }
                    }
                    else if(_receiveAddress == 0x340B)
                    {
                        if(_receiveIndex != 0)
                        {
                            //invalid
                            _invalidReceive = true;
                            _receiveState = 0;
                            _receiveIndex = 0;
                        }
                        else
                        {
                            //maybe set GPIOB
                        }
                    }
                    else if(_receiveAddress == 0x340C)
                    {
                        if(_receiveIndex != 0)
                        {
                            //invalid
                            _invalidReceive = true;
                            _receiveState = 0;
                            _receiveIndex = 0;
                        }
                        else
                        {
                            //maybe set GPIOC
                        }
                    }
                    else if(_receiveAddress > 0x340C && _receiveAddress < 0x3430)
                    {
                        //ADC stuff
                        if(_analogEnableCount == 0)
                        {
                            if(_receiveIndex != 0)
                            {
                                //invalid
                                _invalidReceive = true;
                                _receiveIndex = 0;
                            }
                            _receiveState = 0;
                        }
                        else 
                        {
                            if((_receiveAddress - 0x340C) < _analogEnableCount * 2 + 1)
                            {
                                _receiveData[_receiveIndex] = data[dataIndex];
                                _receiveIndex++;
                            }
                            else
                            {
                                _receiveData[_receiveIndex] = data[dataIndex];
                                //set ADC values
                                uint8_t adcIndex = 1;
                                uint16_t analogEnable = _analogCommandEnableList.front();
                                _analogCommandEnableList.pop_front();
                                if(analogEnable & 0b0000000000000010)
                                {
                                    _registers->AnalogValues[1] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0000000000000100)
                                {
                                    _registers->AnalogValues[2] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0000000000001000)
                                {
                                    _registers->AnalogValues[3] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0000000000010000)
                                {
                                    _registers->AnalogValues[4] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0000000000100000)
                                {
                                    _registers->AnalogValues[5] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0000000001000000)
                                {
                                    _registers->AnalogValues[6] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0000000010000000)
                                {
                                    _registers->AnalogValues[7] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0000000100000000)
                                {
                                    _registers->AnalogValues[8] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0000001000000000)
                                {
                                    _registers->AnalogValues[9] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0000010000000000)
                                {
                                    _registers->AnalogValues[10] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0000100000000000)
                                {
                                    _registers->AnalogValues[11] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0001000000000000)
                                {
                                    _registers->AnalogValues[12] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0010000000000000)
                                {
                                    _registers->AnalogValues[13] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b0100000000000000)
                                {
                                    _registers->AnalogValues[14] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                if(analogEnable & 0b1000000000000000)
                                {
                                    _registers->AnalogValues[15] = _receiveData[adcIndex] | _receiveData[adcIndex + 1];
                                    adcIndex += 2;
                                }
                                _registers->AnalogCounter = _receiveData[0];
                                _receiveIndex = 0;
                                _receiveState = 0;
                            }
                        }
                    }
                    else
                    {
                        _receiveData[_receiveIndex] = data[dataIndex];
                        _receiveIndex++;
                    }

                    _receiveAddress++;
                    break;
            }

            dataIndex++;
        }
    }


    size_t ATTiny427ExpanderUpdateService::transmitAfterMainRead(uint8_t *out, ATTiny427Expander_Registers *transmitRegisters)
    {
		uint8_t bufferIndex = 0;
        
        //Digital
        out[bufferIndex++] = 0x61; //read 1 byte from 8 bit address with 0 as high address byte
		out[bufferIndex++] = 0x0A; //address
		out[bufferIndex++] = 0x61; //read 1 byte from 8 bit address with 0 as high address byte
		out[bufferIndex++] = 0x06; //address
		out[bufferIndex++] = 0x61; //read 1 byte from 8 bit address with 0 as high address byte
		out[bufferIndex++] = 0x02; //address

        //Analog
        bool analogStop = (transmitRegisters->AnalogEnable == 0 && _previousRegisters.AnalogEnable != 0) || _first;
        bool analogStart = transmitRegisters->AnalogEnable != 0 && _previousRegisters.AnalogEnable == 0;
        bool writeGPIOR0 = transmitRegisters->GPIOR0 != _previousRegisters.GPIOR0 || _first;
        bool writeGPIOR1 = transmitRegisters->GPIOR1 != _previousRegisters.GPIOR1 || _first;
        bool writeGPIOR2 = transmitRegisters->GPIOR2 != _previousRegisters.GPIOR2 || _first;

        if(analogStop)
        {
            //todo stop analog
        }

        if(writeGPIOR0)
        {
            out[bufferIndex++] = 0xE3;
            out[bufferIndex++] = 0x1C;
            out[bufferIndex++] = transmitRegisters->GPIOR0;
            out[bufferIndex++] = transmitRegisters->GPIOR1;
            out[bufferIndex++] = transmitRegisters->GPIOR2 | 0x80;
        }
        else if(writeGPIOR1)
        {
            out[bufferIndex++] = 0xE2;
            out[bufferIndex++] = 0x1D;
            out[bufferIndex++] = transmitRegisters->GPIOR1;
            out[bufferIndex++] = transmitRegisters->GPIOR2 | 0x80;
        }
        else if(writeGPIOR2)
        {
            out[bufferIndex++] = 0xE1;
            out[bufferIndex++] = 0x1E;
            out[bufferIndex++] = transmitRegisters->GPIOR2 | (transmitRegisters->AnalogAccumulate != _previousRegisters.AnalogAccumulate? 0x80 : 0x00);
        }

        if(analogStart)
        {
            out[bufferIndex++] = 0x85; //write 16bit address
            out[bufferIndex++] = 0x06; //address high
            out[bufferIndex++] = 0x00; //address low
            out[bufferIndex++] = 0b00100001; //enable ADC with low latency
            out[bufferIndex++] = 0x01; //prescaler DIV4 to get ADCCLK 5MHZ
            out[bufferIndex++] = 0xA0; //set timebase and VDD reference
            out[bufferIndex++] = 0x00; //no window source mode
            out[bufferIndex++] = 0x01; //enable RESRDY interrupt
            out[bufferIndex++] = 0xC3; //write 3 bytes to 8 bit address using existing high byte
            out[bufferIndex++] = 0x08; //address low
            out[bufferIndex++] = 0x05; //SAMPDUR = 5. TODO add this as an adjustable parameter from analogservice
            out[bufferIndex++] = 0x00; //co accumulation. accumulation done in software so the readings are evenly spaced
            out[bufferIndex++] = 0x11; //single 12 bit mode and start
        }

        return bufferIndex;
    }

	size_t ATTiny427ExpanderUpdateService::Transmit(uint8_t out[1024])
	{
        switch(_registers->Comm)
        {
            case ATTiny427Expander_Comm_UART0:
                _registers->PORTB_DIR &= 0b11110011;
                _registers->PORTB_DIR |= 0b00000100;
                _registers->AnalogEnable &= 0xFFFE;
                break;
            case ATTiny427Expander_Comm_UART0Alternate:
            case ATTiny427Expander_Comm_UART1:
                _registers->PORTA_DIR &= 0b11111000;
                _registers->PORTA_DIR |= 0b00000010;
                _registers->AnalogEnable &= 0xFFF8;
                break;
            case ATTiny427Expander_Comm_UART1Alternate:
                _registers->PORTC_DIR &= 0b11111001;
                _registers->PORTC_DIR |= 0b00000100;
                _registers->AnalogEnable &= 0x9FFE;
                break;
            case ATTiny427Expander_Comm_SPI:
                _registers->PORTA_DIR &= 0b11100000;
                _registers->PORTA_DIR |= 0b00000100;
                _registers->AnalogEnable &= 0xFFE0;
                break;
            case ATTiny427Expander_Comm_SPIAlternate:
                _registers->PORTC_DIR &= 0b11110000;
                _registers->PORTC_DIR |= 0b00000010;
                _registers->AnalogEnable &= 0x0FFE;
                break;
        }
        
		uint8_t bufferIndex = 0;

        //read
        size_t mainReadFinishedIndex = 0;
        if(_previousRegisters.AnalogEnable != 0)
        {
            uint8_t analogEnableCount = 0;
            if(_previousRegisters.AnalogEnable & 0b0000000000000010)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0000000000000100)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0000000000001000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0000000000010000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0000000000100000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0000000001000000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0000000010000000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0000000100000000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0000001000000000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0000010000000000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0000100000000000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0001000000000000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0010000000000000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b0100000000000000)
                analogEnableCount++;
            if(_previousRegisters.AnalogEnable & 0b1000000000000000)
                analogEnableCount++;

            out[bufferIndex++] = analogEnableCount + 1;
            out[bufferIndex++] = 0x34;
            if(_previousRegisters.TCB0_CTRLA & 0x1 && _previousRegisters.TCB0_EVCTRL & 0x1)
            {
                out[bufferIndex++] = 0x00;
                mainReadFinishedIndex = bufferIndex + 0xD + analogEnableCount * 2 + 1;
                out[bufferIndex++] = 0x1D;
            }
            else if(_previousRegisters.TCB1_CTRLA & 0x1 && _previousRegisters.TCB1_EVCTRL & 0x1)
            {
                out[bufferIndex++] = 0x05;
                mainReadFinishedIndex = bufferIndex + 0x8 + analogEnableCount * 2 + 1;
                out[bufferIndex++] = 0x18;
            }
            else
            {
                out[bufferIndex++] = 0x0D;
                mainReadFinishedIndex = bufferIndex + analogEnableCount * 2 + 1;
            }
            out[bufferIndex++] = analogEnableCount | 0x10;
            _analogCommandEnableList.push_back(_previousRegisters.AnalogEnable);
        }

		ATTiny427Expander_Registers transmitRegisters = *_registers;

        //Digital
		const bool writePORTC_OUT = transmitRegisters.PORTC_OUT != _previousRegisters.PORTC_OUT || _first;
		const bool writePORTB_OUT = transmitRegisters.PORTB_OUT != _previousRegisters.PORTB_OUT || _first;
		const bool writePORTA_OUT = transmitRegisters.PORTA_OUT != _previousRegisters.PORTA_OUT || _first;
		const bool writePORTC_DIR = transmitRegisters.PORTC_DIR != _previousRegisters.PORTC_DIR || _first;
		const bool writePORTB_DIR = transmitRegisters.PORTB_DIR != _previousRegisters.PORTB_DIR || _first;
		const bool writePORTA_DIR = transmitRegisters.PORTA_DIR != _previousRegisters.PORTA_DIR || _first;
		
		if(writePORTC_OUT)
		{
			out[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			out[bufferIndex++] = 0x09; //address
			out[bufferIndex++] = transmitRegisters.PORTC_OUT; //OUT
		}
        if(mainReadFinishedIndex <= bufferIndex) { bufferIndex += transmitAfterMainRead(&out[bufferIndex], &transmitRegisters); mainReadFinishedIndex = 1337; }
		if(writePORTB_OUT)
		{
			out[bufferIndex++] = writePORTC_DIR? 0xE3 : 0xE1; //write 1 or 3 bytes to 8 bit address with 0 as high address byte
			out[bufferIndex++] = 0x05; //address
			out[bufferIndex++] = transmitRegisters.PORTB_OUT; //OUT
			if(writePORTC_DIR)
			{
				out[bufferIndex++] = 0; //INTFlags. writing 0 to this does nothing
				out[bufferIndex++] = transmitRegisters.PORTC_DIR; //DIR
			}
		}
        if(mainReadFinishedIndex <= bufferIndex) { bufferIndex += transmitAfterMainRead(&out[bufferIndex], &transmitRegisters); mainReadFinishedIndex = 1337; }
		if(writePORTA_OUT)
		{
			out[bufferIndex++] = writePORTB_DIR? 0xE3 : 0xE1; //write 1 or 3 bytes to 8 bit address with 0 as high address byte
			out[bufferIndex++] = 0x01; //address
			out[bufferIndex++] = transmitRegisters.PORTA_OUT; //OUT
			if(writePORTB_DIR)
			{
				out[bufferIndex++] = 0; //INTFlags. writing 0 to this does nothing
				out[bufferIndex++] = transmitRegisters.PORTB_DIR; //DIR
			}
		}
        if(mainReadFinishedIndex <= bufferIndex) { bufferIndex += transmitAfterMainRead(&out[bufferIndex], &transmitRegisters); mainReadFinishedIndex = 1337; }
		if(writePORTC_DIR && !writePORTB_OUT)
		{
			out[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			out[bufferIndex++] = 0x08; //address
			out[bufferIndex++] = transmitRegisters.PORTC_DIR; //DIR

		}
        if(mainReadFinishedIndex <= bufferIndex) { bufferIndex += transmitAfterMainRead(&out[bufferIndex], &transmitRegisters); mainReadFinishedIndex = 1337; }
		if(writePORTB_DIR && !writePORTA_OUT)
		{
			out[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			out[bufferIndex++] = 0x04; //address
			out[bufferIndex++] = transmitRegisters.PORTB_DIR; //DIR
		}
        if(mainReadFinishedIndex <= bufferIndex) { bufferIndex += transmitAfterMainRead(&out[bufferIndex], &transmitRegisters); mainReadFinishedIndex = 1337; }
		if(writePORTA_DIR)
		{
			out[bufferIndex++] = 0xE1; //write 1 byte to 8 bit address with 0 as high address byte
			out[bufferIndex++] = 0x00; //address
			out[bufferIndex++] = transmitRegisters.PORTA_DIR; //DIR
		}
        if(mainReadFinishedIndex <= bufferIndex) { bufferIndex += transmitAfterMainRead(&out[bufferIndex], &transmitRegisters); mainReadFinishedIndex = 1337; }        

        //AC
        const bool writeAC_CTRLA = transmitRegisters.AC_CTRLA != _previousRegisters.AC_CTRLA || _first;
        const bool writeAC_MUXCTRLA = transmitRegisters.AC_MUXCTRLA != _previousRegisters.AC_MUXCTRLA || _first;
        const bool writeAC_DACREF = transmitRegisters.AC_DACREF != _previousRegisters.AC_DACREF || _first;

        if(writeAC_CTRLA)
        {
            out[bufferIndex++] = 0x81 + (writeAC_MUXCTRLA && writeAC_DACREF? 4 : (writeAC_MUXCTRLA? 2 : 0)); //write 16bit address
            out[bufferIndex++] = 0x06; //address high
            out[bufferIndex++] = 0x80; //address low
            out[bufferIndex++] = transmitRegisters.AC_CTRLA; //CTRLA
            if(writeAC_MUXCTRLA)
            {
                out[bufferIndex++] = 0; //nothing byte
                out[bufferIndex++] = transmitRegisters.AC_MUXCTRLA; //MUXCTRLA
                if(writeAC_DACREF)
                {
                    out[bufferIndex++] = 0; //nothing byte
                    out[bufferIndex++] = transmitRegisters.AC_DACREF; //DACREF
                }
            }
            else if(writeAC_DACREF)
            {
                out[bufferIndex++] = 0xC1; //write 1 byte to 8 bit address using existing high byte
                out[bufferIndex++] = 0x82; //low byte
                out[bufferIndex++] = transmitRegisters.AC_DACREF; //DACREF
            }
        }
        else if(writeAC_MUXCTRLA)
        {
            out[bufferIndex++] = 0x81 + (writeAC_DACREF? 2 : 0); //write 16bit address
            out[bufferIndex++] = 0x06; //address high
            out[bufferIndex++] = 0x82; //address low
            out[bufferIndex++] = transmitRegisters.AC_MUXCTRLA; //MUXCTRLA
            if(writeAC_DACREF)
            {
                out[bufferIndex++] = 0; //nothing byte
                out[bufferIndex++] = transmitRegisters.AC_DACREF; //DACREF
            }
        }
        else if(writeAC_DACREF)
        {
            out[bufferIndex++] = 0x81; //write 16bit address
            out[bufferIndex++] = 0x06; //address high
            out[bufferIndex++] = 0x84; //address low
            out[bufferIndex++] = transmitRegisters.AC_DACREF; //DACREF
        }
        if(mainReadFinishedIndex <= bufferIndex) { bufferIndex += transmitAfterMainRead(&out[bufferIndex], &transmitRegisters); mainReadFinishedIndex = 1337; }

        if(mainReadFinishedIndex != 1337)
        {
            while(mainReadFinishedIndex <= bufferIndex) out[bufferIndex++] = 0;

            bufferIndex += transmitAfterMainRead(&out[bufferIndex], &transmitRegisters);
        }

        //EVCTRL

        //TCB0
		const bool writeTCB0_CTRLA = transmitRegisters.TCB0_CTRLA != _previousRegisters.TCB0_CTRLA || _first;
		const bool writeTCB0_CTRLB = transmitRegisters.TCB0_CTRLB != _previousRegisters.TCB0_CTRLB || _first;
		const bool writeTCB0_EVCTRL = transmitRegisters.TCB0_EVCTRL != _previousRegisters.TCB0_EVCTRL || _first;
		const bool writeTCB0_INTCTRL = transmitRegisters.TCB0_INTCTRL != _previousRegisters.TCB0_INTCTRL || _first;

        uint8_t startAddress = 0;
        if(writeTCB0_CTRLA)
            startAddress = 0;
        else if(writeTCB0_CTRLB)
            startAddress = 1;
        else if(writeTCB0_EVCTRL)
            startAddress = 2;
        else if(writeTCB0_INTCTRL)
            startAddress = 3;

        uint8_t length = 0;
        if(writeTCB0_INTCTRL)
            length = 4 - startAddress;
        else if(writeTCB0_EVCTRL)
            length = 3 - startAddress;
        else if(writeTCB0_CTRLB)
            length = 2 - startAddress;
        else if(writeTCB0_CTRLA)
            length = 1 - startAddress;

        if(length > 0)
        {
            out[bufferIndex++] = 0x80 | length; //write 16bit address
            out[bufferIndex++] = 0x0A; //address high
            out[bufferIndex++] = startAddress | 0x80; //address low

            switch(startAddress)
            {
                case 0:
                    out[bufferIndex++] = transmitRegisters.TCB0_CTRLA;
                    if(--length == 0) break;
                case 1:
                    out[bufferIndex++] = transmitRegisters.TCB0_CTRLB;
                    if(--length == 0) break;
                case 2:
                    out[bufferIndex++] = transmitRegisters.TCB0_EVCTRL;
                    if(--length == 0) break;
                case 3:
                    out[bufferIndex++] = transmitRegisters.TCB0_INTCTRL;
                    break;
            }
        }

        //TCB1
		const bool writeTCB1_CTRLA = transmitRegisters.TCB1_CTRLA != _previousRegisters.TCB1_CTRLA || _first;
		const bool writeTCB1_CTRLB = transmitRegisters.TCB1_CTRLB != _previousRegisters.TCB1_CTRLB || _first;
		const bool writeTCB1_EVCTRL = transmitRegisters.TCB1_EVCTRL != _previousRegisters.TCB1_EVCTRL || _first;
		const bool writeTCB1_INTCTRL = transmitRegisters.TCB1_INTCTRL != _previousRegisters.TCB1_INTCTRL || _first;

        startAddress = 0;
        if(writeTCB1_CTRLA)
            startAddress = 0;
        else if(writeTCB1_CTRLB)
            startAddress = 1;
        else if(writeTCB1_EVCTRL)
            startAddress = 2;
        else if(writeTCB1_INTCTRL)
            startAddress = 3;

        length = 0;
        if(writeTCB1_INTCTRL)
            length = 4 - startAddress;
        else if(writeTCB1_EVCTRL)
            length = 3 - startAddress;
        else if(writeTCB1_CTRLB)
            length = 2 - startAddress;
        else if(writeTCB1_CTRLA)
            length = 1 - startAddress;

        if(length > 0)
        {
            out[bufferIndex++] = 0x80 | length; //write 16bit address
            out[bufferIndex++] = 0x0A; //address high
            out[bufferIndex++] = startAddress | 0x90; //address low

            switch(startAddress)
            {
                case 0:
                    out[bufferIndex++] = transmitRegisters.TCB1_CTRLA;
                    if(--length == 0) break;
                case 1:
                    out[bufferIndex++] = transmitRegisters.TCB1_CTRLB;
                    if(--length == 0) break;
                case 2:
                    out[bufferIndex++] = transmitRegisters.TCB1_EVCTRL;
                    if(--length == 0) break;
                case 3:
                    out[bufferIndex++] = transmitRegisters.TCB1_INTCTRL;
                    break;
            }
        }

		_first = false;
		_previousRegisters = transmitRegisters;
		return bufferIndex;
	}
}
#endif

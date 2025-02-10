#include "stdint.h"
#include "stddef.h"
#include <list>

#ifndef ATTINY427EXPANDERUPDATESERVICE_H
#define ATTINY427EXPANDERUPDATESERVICE_H
namespace EmbeddedIOServices
{
	enum ATTiny427Expander_Comm : uint8_t
	{
		ATTiny427Expander_Comm_UART0,
		ATTiny427Expander_Comm_UART0Alternate,
		ATTiny427Expander_Comm_UART1,
		ATTiny427Expander_Comm_UART1Alternate,
		ATTiny427Expander_Comm_SPI,
		ATTiny427Expander_Comm_SPIAlternate
	};

	struct ATTiny427Expander_Registers 
	{
		ATTiny427Expander_Comm Comm;

		//registers
		uint8_t PORTA_DIR = 0b00000100;
		uint8_t PORTA_OUT = 0b00000000;
		uint8_t PORTA_IN = 0b00000000;
		uint8_t PORTB_DIR = 0b00000000;
		uint8_t PORTB_OUT = 0b00000000;
		uint8_t PORTB_IN = 0b00000000;
		uint8_t PORTC_DIR = 0b00000000;
		uint8_t PORTC_OUT = 0b00000000;
		uint8_t PORTC_IN = 0b00000000;
		union
		{
			uint16_t AnalogEnable = 0;
			struct
			{
				uint8_t GPIOR0;
				uint8_t GPIOR1;
			};
		};
		union
		{
			uint8_t GPIOR2 = 0;
			struct
			{
				uint8_t AnalogAccumulate : 4;
				bool SENTenableTCB0 : 1;
				bool SENTenableTCB1 : 1;
				bool GPIOR2_RESERVED : 1;
				bool AnalogAccumulateReset : 1;
			};
		};
		uint8_t PORTMUX_EVSYSROUTEA = 0;
		uint8_t PORTMUX_CCLROUTEA = 0;
		uint8_t AC_CTRLA = 0;
		uint8_t AC_MUXCTRLA = 0;
		uint8_t AC_DACREF = 0xFF;
		uint8_t EVSYS_CHANNEL0 = 0;
		uint8_t EVSYS_CHANNEL1 = 0;
		uint8_t EVSYS_CHANNEL2 = 0;
		uint8_t EVSYS_CHANNEL3 = 0;
		uint8_t EVSYS_CHANNEL4 = 0;
		uint8_t EVSYS_CHANNEL5 = 0;
		uint8_t EVSYS_CCL_LUT0A = 0;
		uint8_t EVSYS_CCL_LUT1A = 0;
		uint8_t EVSYS_CCL_LUT2A = 0;
		uint8_t EVSYS_CCL_LUT3A = 0;
		uint8_t EVSYS_EVOUTA = 0;
		uint8_t EVSYS_EVOUTB = 0;
		uint8_t EVSYS_EVOUTC = 0;
		uint8_t EVSYS_TCB0_CAPT = 0;
		uint8_t EVSYS_TCB1_CAPT = 0;
		uint8_t TCB0_CTRLA = 0;
		uint8_t TCB0_CTRLB = 0;
		uint8_t TCB0_EVCTRL = 0;
		uint8_t TCB0_INTCTRL = 0;
		uint8_t TCB1_CTRLA = 0;
		uint8_t TCB1_CTRLB = 0;
		uint8_t TCB1_EVCTRL = 0;
		uint8_t TCB1_INTCTRL = 0;

		uint8_t CCL_CTRLA = 0;
		uint8_t CCL_LUT0CTRLA = 0;
		uint8_t CCL_LUT0CTRLB = 0;
		uint8_t CCL_LUT0CTRLC = 0;
		uint8_t CCL_LUT0TRUTH = 0;
		uint8_t CCL_LUT1CTRLA = 0;
		uint8_t CCL_LUT1CTRLB = 0;
		uint8_t CCL_LUT1CTRLC = 0;
		uint8_t CCL_LUT1TRUTH = 0;
		uint8_t CCL_LUT2CTRLA = 0;
		uint8_t CCL_LUT2CTRLB = 0;
		uint8_t CCL_LUT2CTRLC = 0;
		uint8_t CCL_LUT2TRUTH = 0;
		uint8_t CCL_LUT3CTRLA = 0;
		uint8_t CCL_LUT3CTRLB = 0;
		uint8_t CCL_LUT3CTRLC = 0;
		uint8_t CCL_LUT3TRUTH = 0;

		//SRAM
		uint8_t AnalogCounter = 0;
		uint16_t AnalogValues[16];
		uint8_t TCB0Counter = 0;
		union
		{
			struct
			{
				uint16_t TCB0CNT;
				uint16_t TCB0CCMP;
			};
			struct
			{
				uint8_t TCB0SENTSTATUS : 4;
				uint16_t TCB0SENTSIGNAL1 : 12;
				uint16_t TCB0SENTSIGNAL2 : 12;
				uint8_t TCB0SENTCRC : 4;
			};
		};
		uint8_t TCB1Counter = 0;
		union
		{
			struct
			{
				uint16_t TCB1CNT;
				uint16_t TCB1CCMP;
			};
			struct
			{
				uint8_t TCB1SENTSTATUS : 4;
				uint16_t TCB1SENTSIGNAL1 : 12;
				uint16_t TCB1SENTSIGNAL2 : 12;
				uint8_t TCB1SENTCRC : 4;
			};
		};
		

		ATTiny427Expander_Registers()
		{
			Comm = ATTiny427Expander_Comm_SPI;
			PORTA_DIR |= 0b00000100;
		}

		ATTiny427Expander_Registers(const ATTiny427Expander_Comm comm) : Comm(comm)
		{
			switch(Comm)
			{
				case ATTiny427Expander_Comm_UART0:
					PORTB_DIR |= 0b00000100;
					break;
				case ATTiny427Expander_Comm_UART0Alternate:
				case ATTiny427Expander_Comm_UART1:
					PORTA_DIR |= 0b00000010;
					break;
				case ATTiny427Expander_Comm_UART1Alternate:
					PORTC_DIR |= 0b00000100;
					break;
				case ATTiny427Expander_Comm_SPI:
					PORTA_DIR |= 0b00000100;
					break;
				case ATTiny427Expander_Comm_SPIAlternate:
					PORTC_DIR |= 0b00000010;
					break;
			}
		}
	};

	class ATTiny427ExpanderUpdateService
	{
	protected:
		ATTiny427Expander_Registers *_registers;
		ATTiny427Expander_Registers _previousRegisters;
		uint16_t _receiveAddress = 0;
		uint8_t _receiveIndex : 5;
		uint8_t _receiveState : 2;
		bool _invalidReceive : 1;
		uint8_t _analogEnableCount : 4;
		bool _first : 1;
		std::list<uint16_t> _analogCommandEnableList;
		uint8_t _receiveData[29];
		size_t transmitAfterMainRead(uint8_t *out, ATTiny427Expander_Registers *transmitRegisters);
	public:
		ATTiny427ExpanderUpdateService(ATTiny427Expander_Registers *registers);
		void Receive(uint8_t *data, size_t size);
		size_t Transmit(uint8_t out[1024]);
	};
}
#endif

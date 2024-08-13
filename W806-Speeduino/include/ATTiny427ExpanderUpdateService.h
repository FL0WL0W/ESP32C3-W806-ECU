#include "IDigitalService.h"
#include <forward_list>

#ifndef ATTINY427EXPANDERUPDATESERVICE_H
#define ATTINY427EXPANDERUPDATESERVICE_H
namespace EmbeddedIOServices
{
	enum ATTiny427Expander_Comm
	{
		UART0,
		UART0Alternate,
		UART1,
		UART1Alternate,
		SPI,
		SPIAlternate
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
			uint8_t GPIOR0 = 0;
			uint8_t AnalogEnable0;
		};
		union
		{
			uint8_t GPIOR1 = 0;
			uint8_t AnalogEnable1;
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
		uint16_t AnalogValues[14];
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
			AlternateSPI = false;
		}

		ATTiny427Expander_Registers(const bool alternateSPI) : AlternateSPI(alternateSPI)
		{
			if(alternateSPI)
				PORTC_DIR = 0b00000010;
			else
				PORTA_DIR = 0b00000100;
		}
	};

	struct ATTiny427Expander_Operations
	{
		//status bits
		bool First : 1;
		bool AnalogRunning : 1;

		//operations
		bool PORTA_OUT : 1;
		bool PORTB_OUT : 1;
		bool PORTC_OUT : 1;
		bool PORTA_DIR : 1;
		bool PORTB_DIR : 1;
		bool PORTC_DIR : 1;
		uint16_t AnalogRead;
		bool AnalogStop : 1;
		bool GPIOR0 : 1;
		bool GPIOR1 : 1;
		bool GPIOR2 : 1;
		bool AnalogStart : 1;
		bool AC_CTRLA : 1;
		bool AC_MUXCTRLA : 1;
		bool AC_DACREF : 1;

		ATTiny427Expander_Operations()
		{
			First = true;
			AnalogRunning = false;
		}
	};

	class ATTiny427ExpanderUpdateService
	{
		ATTiny427Expander_Registers *_registers;
		ATTiny427Expander_Registers _previousRegisters;
		ATTiny427Expander_Operations _operations;
	public:
		ATTiny427ExpanderUpdateService(ATTiny427Expander_Registers *registers);
		size_t Update(uint8_t inOutBuffer[1024]);
	};
}
#endif

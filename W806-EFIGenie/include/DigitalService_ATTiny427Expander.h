#include "IDigitalService.h"
#include <forward_list>

#ifndef DIGITALSERVICE_ATTINY427EXPANDER_H
#define DIGITALSERVICE_ATTINY427EXPANDER_H
namespace EmbeddedIOServices
{
	enum DigitalPort_ATTiny427Expander : uint8_t
	{
		PORTA = 0,
		PORTB = 1,
		PORTC = 2
	};

	typedef uint8_t DigitalPin_ATTiny427Expander;
	
	struct DigitalInterrupt_ATTiny427Expander
	{
		DigitalPort_ATTiny427Expander DigitalPort;
		DigitalPin_ATTiny427Expander DigitalPin;
		callback_t CallBack;

		DigitalInterrupt_ATTiny427Expander(DigitalPort_ATTiny427Expander digitalPort, DigitalPin_ATTiny427Expander digitalPin, callback_t callBack) : DigitalPort(digitalPort), DigitalPin(digitalPin), CallBack(callBack) { }
	};

	typedef std::forward_list<DigitalInterrupt_ATTiny427Expander> DigitalInterruptList_ATTiny427Expander;

	struct DigitalService_ATTiny427Expander_Registers 
	{
		uint8_t PORTA_DIR = 0b00000100;
		uint8_t PORTA_OUT = 0b00000000;
		uint8_t PORTA_IN = 0b00000000;
		uint8_t PORTA_INTFLAGS = 0b00000000; //Not used, but VPORT is organized in this way
		uint8_t PORTB_DIR = 0b00000000;
		uint8_t PORTB_OUT = 0b00000000;
		uint8_t PORTB_IN = 0b00000000;
		uint8_t PORTB_INTFLAGS = 0b00000000; //Not used, but VPORT is organized in this way
		uint8_t PORTC_DIR = 0b00000000;
		uint8_t PORTC_OUT = 0b00000000;
		uint8_t PORTC_IN = 0b00000000;
		uint8_t PORTC_INTFLAGS = 0b00000000; //Not used, but VPORT is organized in this way
	};

	struct DigitalService_ATTiny427Expander_Operations
	{
		bool First : 1;
		bool PORTA_OUT : 1;
		bool PORTB_OUT : 1;
		bool PORTC_OUT : 1;
		bool PORTA_DIR : 1;
		bool PORTB_DIR : 1;
		bool PORTC_DIR : 1;

		DigitalService_ATTiny427Expander_Operations()
		{
			First = true;
		}
	};

	class DigitalService_ATTiny427Expander : public IDigitalService
	{
	protected:
		const bool _altSPI = false;
		DigitalInterruptList_ATTiny427Expander InterruptList;

		DigitalService_ATTiny427Expander_Registers _newRegisters;
		DigitalService_ATTiny427Expander_Registers _previousRegisters;
		DigitalService_ATTiny427Expander_Operations _operations;
	public:
		DigitalService_ATTiny427Expander(const bool altSPI);
		void InitPin(digitalpin_t pin, PinDirection direction);
		bool ReadPin(digitalpin_t pin);
		void WritePin(digitalpin_t pin, bool value);
		void AttachInterrupt(digitalpin_t pin, callback_t callBack);
		void DetachInterrupt(digitalpin_t pin);
		void ConfigurePassthrough(digitalpin_t pinIn, digitalpin_t pinOut, bool inverted);
		size_t Update(uint8_t inOutBuffer[22]);

		static inline DigitalPin_ATTiny427Expander PinToDigitalPin(digitalpin_t pin)
		{
			return 1 << (pin % 8);
		}
		static inline DigitalPort_ATTiny427Expander PinToDigitalPort(digitalpin_t pin)
		{
			return static_cast<DigitalPort_ATTiny427Expander>(pin>>3);
		}
	};
}
#endif

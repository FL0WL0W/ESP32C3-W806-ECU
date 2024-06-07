#include "IDigitalService.h"
#include "wm_regs.h"
#include <forward_list>

#ifndef DIGITALSERVICE_ATTINY427EXPANDER_H
#define DIGITALSERVICE_ATTINY427EXPANDER_H
namespace EmbeddedIOServices
{
	enum GPIOPort_ATTiny427Expander : uint8_t
	{
		PORTA = 0,
		PORTB = 1,
		PORTC = 2
	};

	typedef uint8_t GPIOPin_ATTiny427Expander;
	
	struct DigitalInterrupt_ATTiny427Expander
	{
		GPIOPort_ATTiny427Expander GPIOPort;
		GPIOPin_ATTiny427Expander GPIOPin;
		callback_t CallBack;

		DigitalInterrupt_ATTiny427Expander(GPIOPort_ATTiny427Expander gpioPort, GPIOPin_ATTiny427Expander gpioPin, callback_t callBack) : GPIOPort(gpioPort), GPIOPin(gpioPin), CallBack(callBack) { }
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
		DigitalInterruptList_ATTiny427Expander InterruptList;

		DigitalService_ATTiny427Expander_Registers _newRegisters;
		DigitalService_ATTiny427Expander_Registers _previousRegisters;
		DigitalService_ATTiny427Expander_Operations _operations;
	public:
		DigitalService_ATTiny427Expander();
		~DigitalService_ATTiny427Expander();
		void InitPin(digitalpin_t pin, PinDirection direction);
		bool ReadPin(digitalpin_t pin);
		void WritePin(digitalpin_t pin, bool value);
		void AttachInterrupt(digitalpin_t pin, callback_t callBack);
		void DetachInterrupt(digitalpin_t pin);
		void ConfigurePassthrough(digitalpin_t pinIn, digitalpin_t pinOut);
		size_t Update(uint8_t inOutBuffer[88]);

		static inline GPIOPin_ATTiny427Expander PinToGPIOPin(digitalpin_t pin)
		{
			return 1 << (pin % 8);
		}
		static inline GPIOPort_ATTiny427Expander PinToGPIOPort(digitalpin_t pin)
		{
			return static_cast<GPIOPort_ATTiny427Expander>(pin>>3);
		}
	};
}
#endif

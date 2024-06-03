#include "IDigitalService.h"
#include "wm_regs.h"
#include <forward_list>

#ifndef DIGITALSERVICE_ATTINY427EXPANDER_H
#define DIGITALSERVICE_ATTINY427EXPANDER_H
namespace EmbeddedIOServices
{
	class DigitalService_ATTiny427Expander : public IDigitalService
	{
	public:
		DigitalService_ATTiny427Expander();
		~DigitalService_ATTiny427Expander();
		void InitPin(digitalpin_t pin, PinDirection direction);
		__attribute__((section(".interrupt"))) bool ReadPin(digitalpin_t pin);
		__attribute__((section(".interrupt"))) void WritePin(digitalpin_t pin, bool value);
		void AttachInterrupt(digitalpin_t pin, callback_t callBack);
		void DetachInterrupt(digitalpin_t pin);
		void ConfigurePassthrough(digitalpin_t pinIn, digitalpin_t pinOut);
	};
}
#endif

#include "DigitalService_W80x.h"
#include "DigitalService_ATTiny427Expander.h"
#include "wm_regs.h"
#include <forward_list>

#ifndef DIGITALSERVICE_EG14_H
#define DIGITALSERVICE_EG14_H
namespace EmbeddedIOServices
{
	class DigitalService_EG14 : public IDigitalService
	{
	protected:
		DigitalService_W80x *_digitalService_W80x;
		DigitalService_ATTiny427Expander *_digitalService_U1;
		DigitalService_ATTiny427Expander *_digitalService_U2;
		bool In17 : 1;
		bool In18 : 1;
		bool In19 : 1;
		bool In20 : 1;
		bool In21 : 1;
		bool In22 : 1;
		bool In23 : 1;
		bool In24 : 1;
	public:
		DigitalService_EG14(DigitalService_W80x *digitalService_W80x, DigitalService_ATTiny427Expander *digitalService_U1, DigitalService_ATTiny427Expander *digitalService_U2);
		void InitPin(digitalpin_t pin, PinDirection direction);
		__attribute__((section(".interrupt"))) bool ReadPin(digitalpin_t pin);
		__attribute__((section(".interrupt"))) void WritePin(digitalpin_t pin, bool value);
		void AttachInterrupt(digitalpin_t pin, callback_t callBack);
		void DetachInterrupt(digitalpin_t pin);
	};
}
#endif

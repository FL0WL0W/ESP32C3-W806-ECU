#include "AnalogService_W80x.h"
#include "AnalogService_ATTiny427Expander.h"
#ifndef ANALOGSERVICE_EG14_H
#define ANALOGSERVICE_EG14_H
namespace EmbeddedIOServices
{
	class AnalogService_EG14 : public IAnalogService
	{
	protected:
		AnalogService_W80x *_analogService_W80x;
		AnalogService_ATTiny427Expander *_analogService_U1;
		AnalogService_ATTiny427Expander *_analogService_U2;
	public:
		AnalogService_EG14(AnalogService_W80x *analogService_W80x, AnalogService_ATTiny427Expander *analogService_U1, AnalogService_ATTiny427Expander *analogService_U2);
		void InitPin(analogpin_t pin);
		float ReadPin(analogpin_t pin);
	};
}
#endif

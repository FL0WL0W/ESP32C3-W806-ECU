#include "AnalogService_EG14.h"

#ifdef ANALOGSERVICE_EG14_H
namespace EmbeddedIOServices
{
    AnalogService_EG14::AnalogService_EG14(AnalogService_W80x *analogService_W80x, AnalogService_ATTiny427Expander *analogService_U1, AnalogService_ATTiny427Expander *analogService_U2) :
		_analogService_W80x(analogService_W80x),
		_analogService_U1(analogService_U1),
		_analogService_U2(analogService_U2)
    {
    }
	
	void AnalogService_EG14::InitPin(analogpin_t pin)
	{
        switch(pin)
        {
            case 1:
				_analogService_W80x->InitPin(1);
				break;
            case 2:
				_analogService_W80x->InitPin(4);
				break;
            case 3:
				_analogService_W80x->InitPin(2);
				break;
            case 7:
				_analogService_U1->InitPin(6);
				break;
            case 8:
				_analogService_U1->InitPin(21);
				break;
			case 17:
				_analogService_U2->InitPin(17);
				break;
			case 18:
				_analogService_U2->InitPin(18);
				break;
			case 19:
				_analogService_U2->InitPin(5);
				break;
			case 20:
				_analogService_U2->InitPin(7);
				break;
			case 21:
				_analogService_U1->InitPin(17);
				break;
			case 22:
				_analogService_U1->InitPin(18);
				break;
			case 23:
				_analogService_U1->InitPin(5);
				break;
			case 24:
				_analogService_U1->InitPin(7);
				break;
			case 25:
				_analogService_U2->InitPin(11);
				break;
			case 26:
				_analogService_U1->InitPin(11);
				break;
            case 27:
            case 28:
				_analogService_W80x->InitPin(3);
				break;
			case 49:
				_analogService_U1->InitPin(19);
				break;
            case 50:
				_analogService_U1->InitPin(16);
				break;
            case 51:
				_analogService_U1->InitPin(8);
				break;
            case 52:
				_analogService_U1->InitPin(9);
				break;
            case 53:
				_analogService_U2->InitPin(6);
				break;
            case 54:
				_analogService_U2->InitPin(19);
				break;
            case 55:
				_analogService_U2->InitPin(16);
				break;
            case 56:
				_analogService_U2->InitPin(8);
				break;
            case 57:
				_analogService_U2->InitPin(9);
				break;
            case 58:
				_analogService_U2->InitPin(13);
				break;
		}
	}

	float AnalogService_EG14::ReadPin(analogpin_t pin)
	{
        switch(pin)
        {
            case 1:
				return _analogService_W80x->ReadPin(1);
            case 2:
				return _analogService_W80x->ReadPin(4);
            case 3:
				return _analogService_W80x->ReadPin(2);
            case 7:
				return _analogService_U1->ReadPin(6);
            case 8:
				return _analogService_U1->ReadPin(21);
			case 17:
				return _analogService_U2->ReadPin(17);
			case 18:
				return _analogService_U2->ReadPin(18);
			case 19:
				return _analogService_U2->ReadPin(5);
			case 20:
				return _analogService_U2->ReadPin(7);
			case 21:
				return _analogService_U1->ReadPin(17);
			case 22:
				return _analogService_U1->ReadPin(18);
			case 23:
				return _analogService_U1->ReadPin(5);
			case 24:
				return _analogService_U1->ReadPin(7);
			case 25:
				return _analogService_U2->ReadPin(11);
			case 26:
				return _analogService_U1->ReadPin(11);
            case 27:
            case 28:
				return _analogService_W80x->ReadPin(3) * 5.75; // resistor divider
			case 49:
				return _analogService_U1->ReadPin(19);
            case 50:
				return _analogService_U1->ReadPin(16);
            case 51:
				return _analogService_U1->ReadPin(8);
            case 52:
				return _analogService_U1->ReadPin(9);
            case 53:
				return _analogService_U2->ReadPin(6);
            case 54:
				return _analogService_U2->ReadPin(19);
            case 55:
				return _analogService_U2->ReadPin(16);
            case 56:
				return _analogService_U2->ReadPin(8);
            case 57:
				return _analogService_U2->ReadPin(9);
            case 58:
				return _analogService_U2->ReadPin(13);
		}
	}
}
#endif

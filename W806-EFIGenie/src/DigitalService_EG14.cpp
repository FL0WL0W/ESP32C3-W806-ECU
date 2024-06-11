#include "DigitalService_EG14.h"

#ifdef DIGITALSERVICE_EG14_H
namespace EmbeddedIOServices
{
    DigitalService_EG14::DigitalService_EG14(DigitalService_W80x *digitalService_W80x, DigitalService_ATTiny427Expander *digitalService_U1, DigitalService_ATTiny427Expander *digitalService_U2) :
		_digitalService_W80x(digitalService_W80x),
		_digitalService_U1(digitalService_U1),
		_digitalService_U2(digitalService_U2)
    {
    }
	
	void DigitalService_EG14::InitPin(digitalpin_t pin, PinDirection direction)
	{
        switch(pin)
        {
            case 1:
				_digitalService_W80x->InitPin(6, direction);
				_digitalService_W80x->InitPin(1, In);
				break;
            case 2:
				_digitalService_W80x->InitPin(7, direction);
				_digitalService_W80x->InitPin(4, In);
				break;
            case 3:
				_digitalService_W80x->InitPin(8, direction);
				_digitalService_W80x->InitPin(2, In);
				break;
            case 4:
				_digitalService_W80x->InitPin(9, direction);
				_digitalService_W80x->InitPin(14, In);
				break;
            case 5:
				_digitalService_W80x->InitPin(11, direction);
				_digitalService_W80x->InitPin(32, In);
				break;
            case 6:
				_digitalService_W80x->InitPin(12, direction);
				_digitalService_W80x->InitPin(34, In);
				break;
            case 7:
				_digitalService_W80x->InitPin(15, direction);
				_digitalService_U1->InitPin(6, direction);
				break;
            case 8:
				_digitalService_W80x->InitPin(32, direction);
				_digitalService_U1->InitPin(13, direction);
				break;
            case 9:
				if(direction == Out)
					_digitalService_U2->InitPin(10, Out);
				break;
            case 10:
				if(direction == Out)
					_digitalService_U1->InitPin(10, Out);
				break;
            case 11:
				_digitalService_W80x->InitPin(38, direction);
				_digitalService_W80x->InitPin(39, In); 
				break;
            case 12:
				_digitalService_W80x->InitPin(36, direction);
				_digitalService_W80x->InitPin(37, In);
				break;
            case 17:
				_digitalService_W80x->InitPin(54, direction);
				if((In17 = direction == In))
					_digitalService_U2->ConfigurePassthrough(17, 20, false);
				else
					_digitalService_U2->ConfigurePassthrough(20, 17, true);
				break;
            case 18:
				_digitalService_W80x->InitPin(53, direction);
				if((In18 = direction == In))
					_digitalService_U2->ConfigurePassthrough(18, 15, false);
				else
					_digitalService_U2->ConfigurePassthrough(15, 18, true);
				break;
            case 19:
				_digitalService_W80x->InitPin(50, direction);
				if((In19 = direction == In))
					_digitalService_U2->ConfigurePassthrough(5, 14, false);
				else
					_digitalService_U2->ConfigurePassthrough(14, 5, false);
				break;
            case 20:
				_digitalService_W80x->InitPin(55, direction);
				if((In20 = direction == In))
					_digitalService_U2->ConfigurePassthrough(7, 12, false);
				else
					_digitalService_U2->ConfigurePassthrough(12, 7, true);
				break;
            case 21:
				_digitalService_W80x->InitPin(40, direction);
				if((In21 = direction == In))
					_digitalService_U1->ConfigurePassthrough(17, 20, false);
				else
					_digitalService_U1->ConfigurePassthrough(20, 17, true);
				break;
            case 22:
				_digitalService_W80x->InitPin(43, direction);
				if((In22 = direction == In))
					_digitalService_U1->ConfigurePassthrough(18, 15, false);
				else
					_digitalService_U1->ConfigurePassthrough(15, 18, true);
				break;
            case 23:
				_digitalService_W80x->InitPin(59, direction);
				if((In23 = direction == In))
					_digitalService_U1->ConfigurePassthrough(5, 14, false);
				else
					_digitalService_U1->ConfigurePassthrough(14, 5, false);
				break;
            case 24:
				_digitalService_W80x->InitPin(41, direction);
				if((In24 = direction == In))
					_digitalService_U1->ConfigurePassthrough(7, 12, false);
				else
					_digitalService_U1->ConfigurePassthrough(12, 7, true);
				break;
            case 25:
				if(direction == Out)
					_digitalService_U2->InitPin(21, Out);
				else
				{
					_digitalService_U2->WritePin(10, 0);
					_digitalService_U2->WritePin(21, 0);
					_digitalService_U2->InitPin(10, Out);
					_digitalService_U2->InitPin(21, Out);
					_digitalService_U2->InitPin(11, In);
				}
				break;
            case 26:
				if(direction == Out)
					_digitalService_U1->InitPin(21, Out);
				else
				{
					_digitalService_U1->WritePin(10, 0);
					_digitalService_U1->WritePin(21, 0);
					_digitalService_U1->InitPin(10, Out);
					_digitalService_U1->InitPin(21, Out);
					_digitalService_U1->InitPin(11, In);
				}
				break;
            case 34:
				_digitalService_W80x->InitPin(10, In);
				break;
            case 35:
				_digitalService_W80x->InitPin(13, In);
				break;
            case 36:
				_digitalService_W80x->InitPin(5, In);
				break;
            case 49:
				_digitalService_U1->InitPin(19, In);
				break;
            case 50:
				_digitalService_U1->InitPin(16, In);
				break;
            case 51:
				_digitalService_U1->InitPin(8, In);
				break;
            case 52:
				_digitalService_U1->InitPin(9, In);
				break;
            case 53:
				_digitalService_U2->InitPin(6, In);
				break;
            case 54:
				_digitalService_U2->InitPin(19, In);
				break;
            case 55:
				_digitalService_U2->InitPin(16, In);
				break;
            case 56:
				_digitalService_U2->InitPin(8, In);
				break;
            case 57:
				_digitalService_U2->InitPin(9, In);
				break;
            case 58:
				_digitalService_U2->InitPin(13, In);
				break;
        }
	}
	bool DigitalService_EG14::ReadPin(digitalpin_t pin)
	{
        switch(pin)
        {
            case 1:
				return _digitalService_W80x->ReadPin(1);
            case 2:
				return _digitalService_W80x->ReadPin(4);
            case 3:
				return _digitalService_W80x->ReadPin(2);
            case 4:
				return _digitalService_W80x->ReadPin(14);
            case 5:
				return _digitalService_W80x->ReadPin(32);
            case 6:
				return _digitalService_W80x->ReadPin(34);
            case 7:
				return _digitalService_U1->ReadPin(6);
            case 8:
				return _digitalService_U1->ReadPin(13);
            case 11:
				return _digitalService_W80x->ReadPin(39);
            case 12:
				return _digitalService_W80x->ReadPin(37);
            case 17:
				if(In17)
					return _digitalService_W80x->ReadPin(54);
				return _digitalService_U2->ReadPin(17);
            case 18:
				if(In18)
					return _digitalService_W80x->ReadPin(53);
				return _digitalService_U2->ReadPin(18);
            case 19:
				if(In19)
					return _digitalService_W80x->ReadPin(50);
				return _digitalService_U2->ReadPin(5);
            case 20:
				if(In20)
					return _digitalService_W80x->ReadPin(55);
				return _digitalService_U2->ReadPin(7);
            case 21:
				if(In21)
					return _digitalService_W80x->ReadPin(40);
				return _digitalService_U1->ReadPin(17);
            case 22:
				if(In22)
					return _digitalService_W80x->ReadPin(43);
				return _digitalService_U1->ReadPin(18);
            case 23:
				if(In23)
					return _digitalService_W80x->ReadPin(59);
				return _digitalService_U1->ReadPin(5);
            case 24:
				if(In24)
					return _digitalService_W80x->ReadPin(41);
				return _digitalService_U1->ReadPin(7);
            case 25:
				return _digitalService_U2->ReadPin(11);
            case 26:
				return _digitalService_U1->ReadPin(11);
            case 34:
				return _digitalService_W80x->ReadPin(10);
            case 35:
				return _digitalService_W80x->ReadPin(13);
            case 36:
				return _digitalService_W80x->ReadPin(5);
            case 49:
				return _digitalService_U1->ReadPin(19);
            case 50:
				return _digitalService_U1->ReadPin(16);
            case 51:
				return _digitalService_U1->ReadPin(8);
            case 52:
				return _digitalService_U1->ReadPin(9);
            case 53:
				return _digitalService_U2->ReadPin(6);
            case 54:
				return _digitalService_U2->ReadPin(19);
            case 55:
				return _digitalService_U2->ReadPin(16);
            case 56:
				return _digitalService_U2->ReadPin(8);
            case 57:
				return _digitalService_U2->ReadPin(9);
            case 58:
				return _digitalService_U2->ReadPin(13);
        }
		return false;
	}
	void DigitalService_EG14::WritePin(digitalpin_t pin, bool value)
	{
        switch(pin)
        {
            case 1:
				return _digitalService_W80x->WritePin(6, value);
            case 2:
				return _digitalService_W80x->WritePin(7, value);
            case 3:
				return _digitalService_W80x->WritePin(8, value);
            case 4:
				return _digitalService_W80x->WritePin(9, value);
            case 5:
				return _digitalService_W80x->WritePin(11, value);
            case 6:
				return _digitalService_W80x->WritePin(12, value);
            case 7:
				return _digitalService_W80x->WritePin(15, value);
            case 8:
				return _digitalService_W80x->WritePin(32, value);
            case 9:
				return _digitalService_U2->WritePin(10, value);
            case 10:
				return _digitalService_U1->WritePin(10, value);
            case 11:
				return _digitalService_W80x->WritePin(38, value);
            case 12:
				return _digitalService_W80x->WritePin(36, value);
            case 17:
				return _digitalService_W80x->WritePin(54, !value);
            case 18:
				return _digitalService_W80x->WritePin(53, !value);
            case 19:
				return _digitalService_W80x->WritePin(50, value);
            case 20:
				return _digitalService_W80x->WritePin(55, !value);
            case 21:
				return _digitalService_W80x->WritePin(40, !value);
            case 22:
				return _digitalService_W80x->WritePin(43, !value);
            case 23:
				return _digitalService_W80x->WritePin(59, value);
            case 24:
				return _digitalService_W80x->WritePin(41, !value);
            case 25:
				return _digitalService_U2->WritePin(21, value);
            case 26:
				return _digitalService_U1->WritePin(21, value);
        }
	}
	void DigitalService_EG14::AttachInterrupt(digitalpin_t pin, callback_t callBack)
	{
        switch(pin)
        {
            case 1:
				return _digitalService_W80x->AttachInterrupt(1, callBack);
            case 2:
				return _digitalService_W80x->AttachInterrupt(4, callBack);
            case 3:
				return _digitalService_W80x->AttachInterrupt(2, callBack);
            case 4:
				return _digitalService_W80x->AttachInterrupt(14, callBack);
            case 5:
				return _digitalService_W80x->AttachInterrupt(32, callBack);
            case 6:
				return _digitalService_W80x->AttachInterrupt(34, callBack);
            case 11:
				return _digitalService_W80x->AttachInterrupt(39, callBack);
            case 12:
				return _digitalService_W80x->AttachInterrupt(37, callBack);
            case 17:
				return _digitalService_W80x->AttachInterrupt(54, callBack);
            case 18:
				return _digitalService_W80x->AttachInterrupt(53, callBack);
            case 19:
				return _digitalService_W80x->AttachInterrupt(50, callBack);
            case 20:
				return _digitalService_W80x->AttachInterrupt(55, callBack);
            case 21:
				return _digitalService_W80x->AttachInterrupt(40, callBack);
            case 22:
				return _digitalService_W80x->AttachInterrupt(43, callBack);
            case 23:
				return _digitalService_W80x->AttachInterrupt(59, callBack);
            case 24:
				return _digitalService_W80x->AttachInterrupt(41, callBack);
            case 34:
				return _digitalService_W80x->AttachInterrupt(10, callBack);
            case 35:
				return _digitalService_W80x->AttachInterrupt(13, callBack);
            case 36:
				return _digitalService_W80x->AttachInterrupt(5, callBack);
        }
	}
	void DigitalService_EG14::DetachInterrupt(digitalpin_t pin)
	{
        switch(pin)
        {
            case 1:
				return _digitalService_W80x->DetachInterrupt(1);
            case 2:
				return _digitalService_W80x->DetachInterrupt(4);
            case 3:
				return _digitalService_W80x->DetachInterrupt(2);
            case 4:
				return _digitalService_W80x->DetachInterrupt(14);
            case 5:
				return _digitalService_W80x->DetachInterrupt(32);
            case 6:
				return _digitalService_W80x->DetachInterrupt(34);
            case 11:
				return _digitalService_W80x->DetachInterrupt(39);
            case 12:
				return _digitalService_W80x->DetachInterrupt(37);
            case 17:
				return _digitalService_W80x->DetachInterrupt(54);
            case 18:
				return _digitalService_W80x->DetachInterrupt(53);
            case 19:
				return _digitalService_W80x->DetachInterrupt(50);
            case 20:
				return _digitalService_W80x->DetachInterrupt(55);
            case 21:
				return _digitalService_W80x->DetachInterrupt(40);
            case 22:
				return _digitalService_W80x->DetachInterrupt(43);
            case 23:
				return _digitalService_W80x->DetachInterrupt(59);
            case 24:
				return _digitalService_W80x->DetachInterrupt(41);
            case 34:
				return _digitalService_W80x->DetachInterrupt(10);
            case 35:
				return _digitalService_W80x->DetachInterrupt(13);
            case 36:
				return _digitalService_W80x->DetachInterrupt(5);
        }
	}
}
#endif

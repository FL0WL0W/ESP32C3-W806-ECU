#include "PortMap.h"
#include <map>

std::map<uint8_t, GPIOPort_EIOS *> portMap;

GPIOPort_EIOS *portOutputRegister(uint8_t pin)
{
    GPIOPort_EIOS *ret;
    const std::map<uint8_t, GPIOPort_EIOS *>::iterator pinMap = portMap.find(pin);
    if(pinMap == portMap.end())
        portMap.insert({pin, ret = new GPIOPort_EIOS(pin)});
    else
        ret = pinMap->second;
    return ret;
}
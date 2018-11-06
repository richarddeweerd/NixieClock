#include "PCF8574.h"

#include <Wire.h>
#include <Arduino.h>

PCF8574::PCF8574(const byte deviceAddress)
{
    _address = deviceAddress;
    _dataOut = 0xFF;
}

void PCF8574::write8(byte val){
  Wire.beginTransmission(_address);
  Wire.write(val);
  Wire.endTransmission();
}


void PCF8574::write(const byte pin, const byte value)
{
    if (pin > 7)
    {
        return;
    }
    if (value == LOW)
    {
        _dataOut &= ~(1 << pin);
    }
    else
    {
        _dataOut |= (1 << pin);
    }
    write8(_dataOut);
}

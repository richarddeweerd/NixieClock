#ifndef _PCF8574_H
#define _PCF8574_H

#include <Arduino.h>

class PCF8574
{
public:
    PCF8574(const byte deviceAddress);

    void begin(byte val=0xFF);
    void write8(byte value);
    void write(const byte pin, const byte value);
    byte valueOut() const { return _dataOut; }

private:
    byte _address;
    byte _dataOut;
};


#endif

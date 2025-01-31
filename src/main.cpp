#include <Arduino.h>
#include "three-wire-spi-master.h"
#define CS_n (6)
ThreeWireSPIMaster SPIMaster = ThreeWireSPIMaster();
void setup()
{
    digitalWrite(CS_n, HIGH);
    pinMode(CS_n, OUTPUT);
}

void loop()
{
    uint8_t numOfBytesToSend = 10;
    SPIMaster.sendAndReceive(CS_n, numOfBytesToSend);
}

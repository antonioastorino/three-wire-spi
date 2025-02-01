#include <Arduino.h>
#ifdef MASTER
#include "three-wire-spi-master.h"
#define CS_n (16)

ThreeWireSPIMaster SPIMaster = ThreeWireSPIMaster();

void setup()
{
    Serial.begin(115200);
    pinMode(CS_n, OUTPUT);
    digitalWrite(CS_n, HIGH);
}

void loop()
{
    uint8_t numOfBytesToSend = 10;
    SPIMaster.setOutputBufferAt(0, 0xff);
    SPIMaster.setOutputBufferAt(1, 2);
    SPIMaster.sendAndReceive(CS_n, numOfBytesToSend);
    Serial.print(SPIMaster.getReceivedBufferAt(0), BIN);
    Serial.print(" - ");
    Serial.println(SPIMaster.getReceivedBufferAt(1), BIN);
    delay(2000);
}
#else
// slave
#include "three-wire-spi-slave.h"
void setup()
{
    Serial.begin(115200);
    ThreeWireSPISlaveInit();
}

void loop()
{
    Serial.println("hello");
    delay(1000);
}
#endif

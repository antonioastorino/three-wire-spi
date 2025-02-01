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
    uint8_t numOfBytesToSend = 2;
    uint8_t numOfExpectedBytes = 0;
    SPIMaster.setOutputBufferAt(0, 0x5);
    SPIMaster.setOutputBufferAt(1, 2);
    SPIMaster.sendAndReceive(CS_n, numOfBytesToSend);
    Serial.println("Received:");
    numOfExpectedBytes = SPIMaster.getExpectedNumOfBytes();
    for (uint8_t byteNr = 0;byteNr < numOfExpectedBytes; byteNr++) {  
        Serial.println(SPIMaster.getReceivedBufferAt(byteNr), BIN);
    }
    Serial.println("--------------------");
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
    delay(1000);
}
#endif

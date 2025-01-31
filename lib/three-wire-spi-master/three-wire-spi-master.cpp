#include "three-wire-spi-master.h"
#define PULSE_US (100)
#define CLK (13)
#define DATA (12)

ThreeWireSPIMaster::ThreeWireSPIMaster(void)
{
    digitalWrite(CLK, HIGH);
    pinMode(CLK, OUTPUT);
    pinMode(DATA, INPUT); // keep it at high-z by default
}

uint8_t ThreeWireSPIMaster::__receiveByte()
{
    uint8_t receivedByte = 0;
    for (int i = 0; i < 8; i++)
    {
        digitalWrite(CLK, HIGH);
        delayMicroseconds(PULSE_US);
        receivedByte |= (uint8_t)digitalRead(DATA) << i;
        digitalWrite(CLK, LOW);
        delayMicroseconds(PULSE_US);
    }
    return receivedByte;
}

void ThreeWireSPIMaster::__sendByte(uint8_t byteToSend)
{
    for (int i = 0; i < 8; i++)
    {
        digitalWrite(CLK, HIGH);
        digitalWrite(DATA, byteToSend & 1);
        byteToSend >>= 1;
        delayMicroseconds(PULSE_US);
        digitalWrite(CLK, LOW);
        delayMicroseconds(PULSE_US);
    }
}

void ThreeWireSPIMaster::sendAndReceive(uint8_t CS_n, uint8_t numOfBytesToSend)
{
    digitalWrite(CS_n, LOW);
    pinMode(DATA, OUTPUT);
    delayMicroseconds(400);
    uint8_t expectedNumOfBytes = 0;
    uint8_t byteNumber         = 0;
    ThreeWireSPIMaster::__sendByte(numOfBytesToSend);
    for (byteNumber = 0; byteNumber < numOfBytesToSend; byteNumber++)
    {
        ThreeWireSPIMaster::__sendByte(this->__outputBuffer[byteNumber]);
    }
    // Give the slave the time to process the request
    pinMode(DATA, INPUT);
    delay(1);
    expectedNumOfBytes = ThreeWireSPIMaster::__receiveByte();
    for (byteNumber = 0; byteNumber < expectedNumOfBytes; byteNumber++)
    {
        this->__inputBuffer[byteNumber] = ThreeWireSPIMaster::__receiveByte();
    }
    digitalWrite(CS_n, HIGH);
}

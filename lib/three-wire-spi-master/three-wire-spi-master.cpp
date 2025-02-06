#ifdef MODE_MASTER
#include "three-wire-spi-master.h"
#define PULSE_US (100)

ThreeWireSPIMaster::ThreeWireSPIMaster(uint8_t cs_n)
{
    this->__cs_n = cs_n;
    digitalWrite(this->__cs_n, HIGH);
    pinMode(this->__cs_n, OUTPUT);
    digitalWrite(CLK, HIGH);
    pinMode(CLK, OUTPUT);
    pinMode(DATA, INPUT); // keep it at high-z by default
}

uint8_t ThreeWireSPIMaster::__receiveByte()
{
    uint8_t receivedByte = 0;
    Serial.print("Receiving ");
    for (int i = 0; i < 8; i++)
    {
        Serial.print(".");
        digitalWrite(CLK, HIGH);
        delayMicroseconds(PULSE_US);
        receivedByte |= (uint8_t)digitalRead(DATA) << i;
        digitalWrite(CLK, LOW);
        delayMicroseconds(PULSE_US);
    }
    Serial.println();
    return receivedByte;
}

void ThreeWireSPIMaster::__sendByte(uint8_t byteToSend)
{
    Serial.print("Sending ");
    Serial.println(byteToSend, HEX);
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

void ThreeWireSPIMaster::sendAndReceive(uint8_t numOfBytesToSend)
{
    digitalWrite(this->__cs_n, LOW);
    delayMicroseconds(400);
    this->__expectedNumOfBytes = 0;
    uint8_t byteNumber         = 0;
    pinMode(DATA, OUTPUT);
    ThreeWireSPIMaster::__sendByte(numOfBytesToSend);
    for (byteNumber = 0; byteNumber < numOfBytesToSend; byteNumber++)
    {
        ThreeWireSPIMaster::__sendByte(this->__outputBuffer[byteNumber]);
    }
    // Give the slave the time to process the request
    pinMode(DATA, INPUT);
    delay(100);
    this->__expectedNumOfBytes = ThreeWireSPIMaster::__receiveByte();
    for (byteNumber = 0; byteNumber < this->__expectedNumOfBytes; byteNumber++)
    {
        this->__inputBuffer[byteNumber] = ThreeWireSPIMaster::__receiveByte();
    }
    digitalWrite(this->__cs_n, HIGH);
}

void ThreeWireSPIMaster::setOutputBufferAt(uint8_t byteNumber, uint8_t value)
{
    this->__outputBuffer[byteNumber] = value;
}

uint8_t ThreeWireSPIMaster::getReceivedBufferAt(uint8_t byteNumber)
{
    return this->__inputBuffer[byteNumber];
}

uint8_t ThreeWireSPIMaster::getExpectedNumOfBytes(void) { return this->__expectedNumOfBytes; }
#endif /* MODE_MASTER */

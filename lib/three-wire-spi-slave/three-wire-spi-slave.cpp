#include <Arduino.h>
#ifdef ARDUINO_ARCH_AVR
#include "three-wire-spi-slave.h"

#define SLAVE_CS_n (2)
#define SLAVE_CLK (13)
#define SLAVE_DATA (12)

#define __enableInterruptPortB() PCICR |= B00000001
#define __attachInterruptOnCLK() PCMSK0 |= B00100000
#define __detachInterruptOnCLK() PCMSK0 &= ~(B00100000)

volatile uint8_t __clockCount                    = 0;
volatile uint8_t __inputPayloadByteCount         = 0;
volatile uint8_t __outputPayloadByteCount        = 0;
volatile uint8_t __numOfExpectedBytes            = 0;
volatile uint8_t __numOfBytesToSend              = 0;
volatile uint8_t __inputBuffer[MAX_BUFFER_SIZE]  = {0};
volatile uint8_t __outputBuffer[MAX_BUFFER_SIZE] = {0};
volatile uint8_t __currentOutputByte             = 0;

volatile bool __headerReceived  = false;
volatile bool __requestReceived = false;
volatile bool __headerSent      = false;
static void __processRequest();

static void __enableSPI();

void ThreeWireSPISlaveInit(void)
{
    pinMode(SLAVE_CS_n, INPUT);
    pinMode(SLAVE_CLK, INPUT);
    pinMode(SLAVE_DATA, INPUT); // becomes output when transmitting
    attachInterrupt(digitalPinToInterrupt(SLAVE_CS_n), __enableSPI, CHANGE);
    __enableInterruptPortB();
}

ISR(PCINT0_vect)
{
    if (!__headerReceived && (digitalRead(SLAVE_CLK) == LOW))
    {
        __numOfExpectedBytes |= (uint8_t)digitalRead(SLAVE_DATA) << __clockCount;
        __clockCount++;
        if (__clockCount == 8)
        {
            __headerReceived = true;
            __clockCount     = 0;
        }
        return;
    }
    if (!__requestReceived && (digitalRead(SLAVE_CLK) == LOW))
    {
        Serial.print("Payload count ");
        Serial.println(__inputPayloadByteCount);
        __inputBuffer[__inputPayloadByteCount] |= (uint8_t)digitalRead(SLAVE_DATA) << __clockCount;
        __clockCount++;
        if (__clockCount == 8)
        {
            __clockCount = 0;
            __inputPayloadByteCount += 1;
        }
        if (__inputPayloadByteCount == __numOfExpectedBytes)
        {
            __requestReceived = true;
            __processRequest();
        }
        return;
    }
    if (__requestReceived && (digitalRead(SLAVE_CLK) == HIGH))
    {

        Serial.print("byte: ");
        Serial.print(__outputPayloadByteCount);
        Serial.print(" of: ");
        Serial.println(__numOfBytesToSend);
        if (__outputPayloadByteCount > __numOfBytesToSend)
        {
            __detachInterruptOnCLK();
            Serial.println("This should not happen: we are asked to send more bytes than intended");
            return;
        }
        digitalWrite(SLAVE_DATA, __currentOutputByte & 1);
        __clockCount++;
        __currentOutputByte >>= 1;
        if (__clockCount == 8)
        {
            if (!__headerSent)
            {
                __headerSent = true;
            }
            else
            {
                __outputPayloadByteCount += 1;
            }
            __clockCount = 0;
            __currentOutputByte = __outputBuffer[__outputPayloadByteCount];
        }
    }
}

static void __enableSPI()
{
    if (digitalRead(SLAVE_CS_n))
    {
        __detachInterruptOnCLK();
        pinMode(SLAVE_DATA, INPUT); // go tristate
    }
    else
    {
        __numOfExpectedBytes    = 0;
        __clockCount            = 0;
        __inputPayloadByteCount = 0;
        __headerReceived        = false;
        __requestReceived       = false;
        __headerSent            = false;
        __attachInterruptOnCLK();
    }
}

void __processRequest()
{
    pinMode(SLAVE_DATA, OUTPUT); // own the bus
    Serial.print("Expected bytes: ");
    Serial.println(__numOfExpectedBytes);
    if (__numOfExpectedBytes == 0)
    {
        return;
    }
    if (__inputBuffer[0] == 0xff)
    {
        // LOOP back
        memcpy((void*)__outputBuffer, (const void*)__inputBuffer, __numOfExpectedBytes);
        __numOfBytesToSend = __numOfExpectedBytes;
    }
    else
    {
        // TODO: Fill in the output buffer with a meaningful answer.
        __outputBuffer[0]  = 0xa5;
        __outputBuffer[1]  = 0xa5;
        __outputBuffer[2]  = 0xa5;
        __numOfBytesToSend = 3;
    }
    __currentOutputByte      = __numOfBytesToSend; // this is the first byte that will be sent
    __outputPayloadByteCount = 0;
    Serial.print("Sending bytes: ");
    Serial.println(__numOfBytesToSend);
}
#else
#error Unsupported platform
#endif

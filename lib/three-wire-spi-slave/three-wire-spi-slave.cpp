#ifdef MODE_SLAVE
#include <Arduino.h>
#include "common.h"
#include "three-wire-spi-slave.h"

#define __enableInterruptPortB() PCICR |= B00000001
#define __attachInterruptOnCLK() PCMSK0 |= B00100000
#define __detachInterruptOnCLK() PCMSK0 &= ~(B00100000)

static volatile uint8_t __clockCount;
static volatile uint8_t __inputPayloadByteCount;
static volatile uint8_t __outputPayloadByteCount;
static volatile uint8_t __numOfExpectedBytes;
static volatile uint8_t __numOfBytesToSend;
static volatile uint8_t __currentOutputByte;
static volatile State __state = IDLE;

volatile uint8_t ThreeWireSPISlave::inputBuffer[MAX_BUFFER_SIZE]  = {0};
volatile uint8_t ThreeWireSPISlave::outputBuffer[MAX_BUFFER_SIZE] = {0};

static void __enableSPI();

void ThreeWireSPISlave::init(void)
{
    pinMode(CS_n, INPUT);
    pinMode(CLK, INPUT);
    pinMode(DATA, INPUT); // becomes output when transmitting
    attachInterrupt(digitalPinToInterrupt(CS_n), __enableSPI, CHANGE);
    __enableInterruptPortB();
}

State ThreeWireSPISlave::getState(void) { return __state; }

uint8_t ThreeWireSPISlave::getNumberOfReceivedBytes(void) { return __inputPayloadByteCount; }

void ThreeWireSPISlave::sendResponse(uint8_t bytesToSend)
{
    pinMode(DATA, OUTPUT); // DATA becomes MISO
    __state = REQUEST_PROCESSED;
    __numOfBytesToSend  = bytesToSend;
    __currentOutputByte = __numOfBytesToSend; // this is the first byte that will be sent
}

ISR(PCINT0_vect)
{
    if (__state == RECEIVING_HEADER && (digitalRead(CLK) == LOW))
    {
        __numOfExpectedBytes |= (uint8_t)digitalRead(DATA) << __clockCount;
        __clockCount = (__clockCount + 1) & 0b111;
        if (__clockCount == 0)
        {
            if (__numOfExpectedBytes == 0)
            {
                // No payload attached
                __state = REQUEST_RECEIVED;
            }
            else
            {
                // Should receive payload
                __state = HEADER_RECEIVED;
            }
        }
    }
    else if (__state == HEADER_RECEIVED && (digitalRead(CLK) == LOW))
    {
        ThreeWireSPISlave::inputBuffer[__inputPayloadByteCount] |= (uint8_t)digitalRead(DATA)
                                                                   << __clockCount;
        __clockCount = (__clockCount + 1) & 0b111;
        if (__clockCount == 0)
        {
            __inputPayloadByteCount += 1;
            if (__inputPayloadByteCount == __numOfExpectedBytes)
            {
                __state = REQUEST_RECEIVED;
            }
        }
    }
    else if (__state == REQUEST_PROCESSED && (digitalRead(CLK) == HIGH))
    {
        digitalWrite(DATA, __currentOutputByte & 1);
        __clockCount = (__clockCount + 1) & 0b111;
        __currentOutputByte >>= 1;
        if (__clockCount == 0)
        {
            if (__numOfBytesToSend == 0)
            {
                __state = RESPONSE_SENT;
            }
            else
            {
                __state             = HEADER_SENT;
                __currentOutputByte = ThreeWireSPISlave::outputBuffer[0];
            }
        }
    }
    else if (__state == HEADER_SENT && (digitalRead(CLK) == HIGH))
    {
        digitalWrite(DATA, __currentOutputByte & 1);
        __currentOutputByte >>= 1;
        __clockCount = (__clockCount + 1) & 0b111;
        if (__clockCount == 0)
        {
            __outputPayloadByteCount += 1;
            if (__outputPayloadByteCount == __numOfBytesToSend)
            {
                __state = RESPONSE_SENT;
            }
            else
            {
                __currentOutputByte = ThreeWireSPISlave::outputBuffer[__outputPayloadByteCount];
            }
        }
    }
    else if (__state == RESPONSE_SENT)
    {
        __detachInterruptOnCLK();
        Serial.print("Response sent - bytes: ");
        Serial.println(__outputPayloadByteCount);
        __state = IDLE;
    }
}

static void __enableSPI()
{
    if (digitalRead(CS_n))
    {
        __detachInterruptOnCLK();
        pinMode(DATA, INPUT); // go tristate
        __state = IDLE;
    }
    else
    {
        __numOfExpectedBytes    = 0;
        __clockCount            = 0;
        __inputPayloadByteCount = 0;
        __state                 = RECEIVING_HEADER;
        __attachInterruptOnCLK();
        __outputPayloadByteCount = 0;
    }
}

#endif /* MODE_SLAVE */

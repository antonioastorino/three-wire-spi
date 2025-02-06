#include <Arduino.h>

#if defined(MODE_MASTER)
/* MASTER
 * This sketch initializes the 3-wire-SPI interface as MASTER and periodically transmits messages to
 * SLAVE.
 */
#include "three-wire-spi-master.h"
// There could be more than one slave, each of which is attached to their respective MASTER's CS_n
// pin. Instead of creating several instances of MASTER, one for each slave, one instance is created
// and the required CS_n pin is send as a parameter to 'sendAndReceive()' method.

#if defined(ARCH_PRO_MICRO)
#define CS1_n (16)
#elif defined(ARCH_UNO)
#define CS1_n (10)
#endif /* defined(ARCH_...) */

void setup()
{
    digitalWrite(CS1_n, HIGH);
    pinMode(CS1_n, OUTPUT);
    ThreeWireSPIMaster::init();
    Serial.begin(115200);
}

uint8_t testNum = 0;
#define NUM_OF_TEST (2);
void loop()
{
    uint8_t receivedBytes = 0;
    switch (testNum)
    {
    case 0:
    {
        // Test one - PING
        // Sending 0 as number of bytes is interpreted as PING - expected response 0xa5
        ThreeWireSPIMaster::sendAndReceive(CS1_n, 0);
        receivedBytes = ThreeWireSPIMaster::getExpectedNumOfBytes();
        if (receivedBytes != 1)
        {
            Serial.print("ERROR: expecting a 1-byte response, got ");
            Serial.println(receivedBytes);
        }
        else if (ThreeWireSPIMaster::getReceivedBufferAt(0) != 0xa5)
        {
            Serial.print("ERROR: expecting a `0xa5` response, got ");
            Serial.println(ThreeWireSPIMaster::getReceivedBufferAt(0));
        }
        else
        {
            Serial.println("SUCCESS: PING test passed");
        }
    }
    break;
    case 1:
    {
        ThreeWireSPIMaster::setOutputBufferAt(0, CMD_LOOPBAK);
        ThreeWireSPIMaster::setOutputBufferAt(1, 0xaf);
        ThreeWireSPIMaster::setOutputBufferAt(2, 0xfa);
        ThreeWireSPIMaster::sendAndReceive(CS1_n, 3);
        receivedBytes = ThreeWireSPIMaster::getExpectedNumOfBytes();
        if (receivedBytes != 3)
        {
            Serial.print("ERROR: Expecting 3 bytes, response, got ");
            Serial.println(receivedBytes);
        }
        else if (
            ThreeWireSPIMaster::getReceivedBufferAt(0) == CMD_LOOPBAK
            && ThreeWireSPIMaster::getReceivedBufferAt(1) == 0xaf
            && ThreeWireSPIMaster::getReceivedBufferAt(2) == 0xfa)
        {
            Serial.println("SUCCESS: Loopback test passed");
        }
        else
        {
            Serial.println("ERROR: wrong response received");
        }

        Serial.println("Received: ");
        for (uint8_t byteNr = 0; byteNr < receivedBytes; byteNr++)
        {
            Serial.println(ThreeWireSPIMaster::getReceivedBufferAt(byteNr), HEX);
        }
    }
    break;
    default:
        Serial.print("Invalid test case ");
        Serial.println(testNum);
    }

    Serial.println("--------------------");
    testNum = (testNum + 1) % NUM_OF_TEST;
    delay(2000);
}

#elif defined(MODE_SLAVE)
/* SLAVE
 * This sketch initializes the 3-wire-SPI interface as SLAVE, waits for messages from MASTER, and
 * responds to them accordingly.
 */
#include "three-wire-spi-slave.h"

void setup()
{
    Serial.begin(115200);
    ThreeWireSPISlave::init();
}

void loop()
{
    uint8_t numOfBytesToSend = 0;
    uint8_t receivedBytes    = 0;
    while (ThreeWireSPISlave::getState() != REQUEST_RECEIVED)
    {
        delay(1);
    }

    numOfBytesToSend = 0;
    receivedBytes    = ThreeWireSPISlave::getNumberOfReceivedBytes();

    Serial.println("--------------------");
    Serial.print("Received bytes: ");
    Serial.println(receivedBytes);
    if (receivedBytes == 0)
    {
        // An empty payload is interpreted as a "ping" command. Respond with the magic byte 0xa5
        ThreeWireSPISlave::outputBuffer[0] = 0xa5;
        numOfBytesToSend                   = 1;
    }
    else if (ThreeWireSPISlave::inputBuffer[0] == 0xaa)
    {
        // LOOP back
        memcpy(
            (void*)ThreeWireSPISlave::outputBuffer,
            (const void*)ThreeWireSPISlave::inputBuffer,
            receivedBytes);
        numOfBytesToSend = receivedBytes;
    }
    else
    {
        // TODO: Fill in the output buffer with a meaningful answer.
        ThreeWireSPISlave::outputBuffer[0] = 0xa5;
        ThreeWireSPISlave::outputBuffer[1] = 0xa5;
        ThreeWireSPISlave::outputBuffer[2] = 0xa5;
        numOfBytesToSend                   = 3;
    }
    ThreeWireSPISlave::sendResponse(numOfBytesToSend);
    Serial.print("Sending bytes: ");
    Serial.println(numOfBytesToSend);
    while (ThreeWireSPISlave::getState() != IDLE)
    {
        ;
    }
}

#else
#error "Please define MODE_MASTER or MODE_SLAVE flags."
#endif /* defined(MODE...) */

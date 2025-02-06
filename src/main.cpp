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

void loop()
{
    uint8_t numOfBytesToSend = 2;
    uint8_t receivedBytes    = 0;
    ThreeWireSPIMaster::setOutputBufferAt(0, 0x5);
    ThreeWireSPIMaster::setOutputBufferAt(1, 2);
    ThreeWireSPIMaster::sendAndReceive(CS1_n, numOfBytesToSend);
    receivedBytes = ThreeWireSPIMaster::getExpectedNumOfBytes();
    Serial.print("Should receive bytes: ");
    Serial.println(receivedBytes);
    for (uint8_t byteNr = 0; byteNr < receivedBytes; byteNr++)
    {
        Serial.println(ThreeWireSPIMaster::getReceivedBufferAt(byteNr), BIN);
    }
    Serial.println("--------------------");
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
        ThreeWireSPISlave::outputBuffer[0] == 0xa5;
        numOfBytesToSend = 1;
    }
    else if (ThreeWireSPISlave::inputBuffer[0] == 0xff)
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

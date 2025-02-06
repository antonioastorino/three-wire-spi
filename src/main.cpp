#include <Arduino.h>

#if defined(MODE_MASTER)
/* MASTER
 * This sketch initializes the 3-wire-SPI interface as MASTER and periodically transmits messages to
 * SLAVE.
 */
#include "three-wire-spi-master.h"
// There could be more than one slave. For each of them, an instance of ThreeWireSPIMaster should be
// created, each of which uses a different CS_n pin. Therefore, it's best to have all the CS pins
// defined here, in the main file.
#if defined(ARCH_PRO_MICRO)
#define CS1_n (16)
#elif defined(ARCH_UNO)
#define CS1_n (10)
#endif /* defined(ARCH_...) */

ThreeWireSPIMaster SPIMaster = ThreeWireSPIMaster(CS1_n);

void setup() { Serial.begin(115200); }

void loop()
{
    uint8_t numOfBytesToSend   = 2;
    uint8_t numOfExpectedBytes = 0;
    SPIMaster.setOutputBufferAt(0, 0x5);
    SPIMaster.setOutputBufferAt(1, 2);
    SPIMaster.sendAndReceive(numOfBytesToSend);
    Serial.println("Received:");
    numOfExpectedBytes = SPIMaster.getExpectedNumOfBytes();
    for (uint8_t byteNr = 0; byteNr < numOfExpectedBytes; byteNr++)
    {
        Serial.println(SPIMaster.getReceivedBufferAt(byteNr), BIN);
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
    ThreeWireSPISlaveInit();
}

void loop() { delay(1000); }
#else
#error "Please define MODE_MASTER or MODE_SLAVE flags."
#endif /* defined(MODE...) */

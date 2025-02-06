#ifndef THREE_WIRE_SPI_MASTER
#define THREE_WIRE_SPI_MASTER
#include <Arduino.h>
#include "common.h"

class ThreeWireSPIMaster
{
private:
    // We can send and receive 0 to 255 bytes - max 32 bytes
    static uint8_t __outputBuffer[MAX_BUFFER_SIZE];
    static uint8_t __inputBuffer[MAX_BUFFER_SIZE];
    static uint8_t __expectedNumOfBytes;
    static void __sendByte(uint8_t);
    static uint8_t __receiveByte(void);

public:
    ThreeWireSPIMaster() = delete;
    static void init(void);
    static void sendAndReceive(uint8_t cs_n, uint8_t bytesToSend);
    static void setOutputBufferAt(uint8_t byteNumber, uint8_t value);
    static uint8_t getReceivedBufferAt(uint8_t byteNumber);
    static uint8_t getExpectedNumOfBytes(void);
};
#endif /* THREE_WIRE_SPI_MASTER */

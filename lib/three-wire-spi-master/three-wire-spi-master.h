#ifndef THREE_WIRE_SPI_MASTER
#define THREE_WIRE_SPI_MASTER
#include <Arduino.h>
#include "common.h"

class ThreeWireSPIMaster
{
private:
    // We can send and receive 0 to 255 bytes - max 32 bytes
    uint8_t __outputBuffer[MAX_BUFFER_SIZE];
    uint8_t __inputBuffer[MAX_BUFFER_SIZE];
    static void __sendByte(uint8_t);
    static uint8_t __receiveByte(void);

public:
    ThreeWireSPIMaster();

    void initialize(void);
    void sendAndReceive(uint8_t CS_n, uint8_t bytesToSend);
    void setOutputBufferAt(uint8_t byteNumber, uint8_t value); 
    uint8_t getReceivedBufferAt(uint8_t byteNumber);
};
#endif /* THREE_WIRE_SPI_MASTER */

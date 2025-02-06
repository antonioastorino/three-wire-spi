#ifndef THREE_WIRE_SPI_SLAVE_H
#define THREE_WIRE_SPI_SLAVE_H
#include "common.h"
enum State
{
    IDLE = 0,
    RECEIVING_HEADER,
    HEADER_RECEIVED,
    REQUEST_RECEIVED,
    REQUEST_PROCESSED,
    HEADER_SENT,
    RESPONSE_SENT,
};

class ThreeWireSPISlave
{
public:
    static volatile uint8_t inputBuffer[MAX_BUFFER_SIZE];
    static volatile uint8_t outputBuffer[MAX_BUFFER_SIZE];

    static void init(void);
    static State getState(void);
    static uint8_t getNumberOfReceivedBytes(void);
    static void sendResponse(uint8_t bytesToSend);
};

#endif /* THREE_WIRE_SPI_SLAVE_H */

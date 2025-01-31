# 3-wire SPI

Software SPI protocol using a 3-wire SPI bus, wherein MISO and MOSI use the same physical DATA line.

Master initiates a conversation with a HIGH-to-LOW transition of the CS_n pin. Master outputs to the DATA line while Slave reads from it (DATA is MOSI). The first transmitter byte contains the number of bits that Master intends to send afterwards. When Master is done transmitting, Master starts to listen on the DATA line while Slave uses it to transmit (DATA is MISO). As before, Slave sends the number of bits it wants to transmit as a first byte. The transmission ends successfully if Master receives all the bits that Slave promised to send as response.

Master handles the timeout in case Slave does not respond promptly or if the promised bits are not received. Master always handles the clock.

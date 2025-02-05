# 3-wire SPI

## Description

Software SPI protocol using a 3-wire SPI bus, wherein `MISO` and `MOSI` use the same physical `DATA` line.
Intended for interfacing one MASTER uC with one or more SLAVE uC, as shown below.

```
MASTER                        SLAVE(s)
  |              CLK             |
  |  ----------------------->    | (interrupt pin)
  |              CS_n            |
  |  ----------------------->    | (interrupt pin) 
  |              DATA            |
  |  <---------------------->    |
```

If you prefer using 4 wires instead of 3, you can easily modify this library by adding an extra line to both MASTER and SLAVE.
However, because the communication is *simplex*, an extra line would not be beneficial except for one specific case, described in the [Limitations](https://github.com/antonioastorino/three-wire-spi?tab=readme-ov-file#limitations) section.

## Purpose
This library was created to solve the problems described in this section.

### SPI wrapper
MASTER needs to talk to several serial devices that don't "speak" SPI (e.g., the SIPO register 74HC595, which requires an extra clock to update the output.)
Instead of connecting MASTER to such devices, a uC can be interposed as a "wrapper".

### Modularity
MASTER needs to handle several things and it's code is getting very crowded.
Delegating some logic to SLAVE could be beneficial, especially by designing the circuit so that SLAVE can be **programmed and tested independently.**

### Expanding device functionalities
Assuming you want to read and filter an analog quantity. For that, you use and external ADC. MASTER is not interested in the readings but only on the filtered data. However, the ADC itself does not provide the desired filtering function. One approach is the following.

<p align=center>MASTER does everything</p>

```
  +-------------------+
  |      MASTER       |         +---------+
  |  - read ADC       |   SPI   |   ADC   |
  |  - filter data    | <-----> |         |
  |  - do my things   |         +---------+
  +-------------------+
```

Using this library, a second approach is available.
<p align=center>MASTER sees an ADC with advanced filtering</p>

```
                                        +-------------------------------------+
  +----------------------+              |          ADC with filter            |
  |       MASTER         |              | +---------------+         +-------+ |                
  | - read filtered data |  3-wire SPI  | |    SLAVE      |   SPI   |  ADC  | |
  | - do my things       | <----------> | | - read ADC    | <-----> |       | |
  |                      |              | | - filter data |         |       | |
  +----------------------+              | +---------------+         +-------+ |
                                        +-------------------------------------+
```
## Protocol description
- The `DATA` line is initially driven by MASTER while SLAVE sets the corresponding pin to `INPUT` to avoid conflicts on the bus.
- MASTER initiates a conversation with a HIGH-to-LOW transition of `CS_n`.
- MASTER outputs to the DATA line while SLAVE reads from it (`DATA` works as `MOSI`), upon falling-edge events.
- The first transmitted byte contains the number of bytes that MASTER intends to send afterwards, maximum 255. This allows SLAVE to know when to start sending, instead of receiving.
- When MASTER is done transmitting, MASTER sets its `DATA` pin to `INPUT`.
- MASTER waits a few milliseconds to give SLAVE the chance to process the received data.
- Meanwhile, SLAVE sets its `DATA` pin to `OUTPUT` and computes the response (`DATA` is `MISO`).
- SLAVE updates `DATA` upon rising-edge events.
- Upon the next 8 clocks, Slave sends the number of bytes it wants to transmit (max 255). Now MASTER know how many bites to expect.
- MASTER continue to send clock pulses and to read the `DATA` line.
- Once the transmission is over, MASTER sets the `CS_n` line to logic `HIGH` and SLAVE sets the `DATA` pin to `INPUT`.

## Limitations
If MASTER and SLAVE do not use the same logic levels, the 3-wire implementation cannot be adopted.
For example, assuming that MASTER works at 3.3 V while SLAVE works at 5 V, the `CLK` and `CS_n` lines can be voltage-divided but the `DATA` line should be voltage-divided only when working as `MOSI`.
This can certainly be achieved by adding some extra electronics, but it would be better to use a separate line for `MISO` in this case.

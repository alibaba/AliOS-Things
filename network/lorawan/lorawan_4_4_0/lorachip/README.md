README.md: lorachip

## Contents

lorachip
├── Config.in
├── aos.mk
├── sx1276
│   ├── sx1276.c
│   ├── sx1276.h
│   ├── sx1276Regs-Fsk.h
│   └── sx1276Regs-LoRa.h
├── ucube.py


## Introduction

This is lorachip component, currently only support sx1276. 

### Dependencies

* lorawan compoent

## API

### SX1276Init
```
void SX1276Init( RadioEvents_t *events )
```
Initializes the radio.

#### Arguments
| name | type | description |
| --- | --- | --- |
| events | RadioEvents_t | Structure containing the driver callback functions |

#### Return
| return | description |
| --- | --- |
| void| |

### SX1276GetStatus
```
RadioState_t SX1276GetStatus( void )
```
Return current radio status.

#### Arguments
| name | type | description |
| --- | --- | --- |
| void | void | |

#### Return
| return | description |
| --- | --- |
| RF_IDLE| RF is in IDLE mode  |
| RF_RX_RUNNING| RF is in receiving data  |
| RF_TX_RUNNING| RF is in transmitting data  |

### SX1276SetModem

```
void SX1276SetModem( RadioModems_t modem )
```
Configures the radio with the given modem.

#### Arguments
| name | type | description |
| --- | --- | --- |
| modem| RadioModems_t | Modem to be used [0: FSK, 1: LoRa] |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276SetChannel

```
void SX1276SetChannel( uint32_t freq )
```
Sets the channel configuration.

#### Arguments
| name | type | description |
| --- | --- | --- |
| freq| uint32_t| Channel RF frequency |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276IsChannelFree

```
bool SX1276IsChannelFree( RadioModems_t modem, uint32_t freq, int16_t rssiThresh, uint32_t maxCarrierSenseTime )
```
Checks if the channel is free for the given time.

#### Arguments
| name | type | description |
| --- | --- | --- |
| modem| RadioModems_t| Radio modem to be used [0: FSK, 1: LoRa] |
| freq| uint32_t| Channel RF frequency |
| rssiThresh| int16_t| RSSI threshold |
| maxCarrierSenseTime | uint32_t | Max time while the RSSI is measured |

#### Return
| return | description |
| --- | --- |
| bool | true: Channel is free, false: Channel is not free |

### SX1276Random

```
uint32_t SX1276Random( void )
```
Generates a 32 bits random value based on the RSSI readings.

#### Arguments
| name | type | description |
| --- | --- | --- |
| void| void |  |


#### Return
| return | description |
| --- | --- |
| randomValue | 32 bits random value |

### SX1276SetRxConfig

```
void SX1276SetRxConfig( RadioModems_t modem, uint32_t bandwidth, uint32_t datarate, uint8_t coderate, uint32_t bandwidthAfc, uint16_t preambleLen, uint16_t symbTimeout, bool fixLen, uint8_t payloadLen, bool crcOn, bool freqHopOn, uint8_t hopPeriod, bool iqInverted, bool rxContinuous )
```
Sets the reception parameters.

#### Arguments
| name | type | description |
| --- | --- | --- |
| modem | RadioModems_t | Radio modem to be used [0: FSK, 1: LoRa] |
| bandwidth| uint32_t | Sets the bandwidth |
| datarate| uint32_t | Sets the Datarate |
| coderate | uint8_t | Sets the coding rate (LoRa only) |
| bandwidthAfc | uint32_t | Sets the AFC Bandwidth (FSK only) |
| preambleLen| uint16_t | Sets the Preamble length |
| symbTimeout| uint16_t | Sets the RxSingle timeout value |
| fixLen | bool | Fixed length packets [0: variable, 1: fixed] |
| payloadLen | uint8_t | Sets payload length when fixed length is used |
| crcOn | bool | Enables/Disables the CRC [0: OFF, 1: ON] |
| freqHopOn | bool | Enables disables the intra-packet frequency hopping |
| hopPeriod | uint8_t | Number of symbols between each hop |
| iqInverted | bool | Inverts IQ signals (LoRa only) |
| rxContinuous | bool | Sets the reception in continuous mode |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276SetTxConfig

```
void SX1276SetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev, uint32_t bandwidth, uint32_t datarate, uint8_t coderate, uint16_t preambleLen, bool fixLen, bool crcOn, bool freqHopOn, uint8_t hopPeriod, bool iqInverted, uint32_t timeout )
```
Sets the transmission parameters.

#### Arguments
| name | type | description |
| --- | --- | --- |
| modem | RadioModems_t | Radio modem to be used [0: FSK, 1: LoRa] |
| power | int8_t | Sets the output power [dBm] |
| fdev | uint32_t | Sets the frequency deviation (FSK only) |
| bandwidth | uint32_t | Sets the bandwidth (LoRa only) |
| datarate | uint32_t | Sets the Datarate |
| coderate | uint8_t | Sets the coding rate (LoRa only) |
| preambleLen | uint16_t | Sets the preamble length |
| fixLen | bool | Fixed length packets [0: variable, 1: fixed] |
| crcOn | bool | Enables disables the CRC [0: OFF, 1: ON] |
| freqHopOn  | bool | Enables disables the intra-packet frequency hopping |
| hopPeriod | uint8_t | Number of symbols between each hop |
| iqInverted | bool | Inverts IQ signals (LoRa only) |
| timeout | uint32_t| Transmission timeout |


#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276GetTimeOnAir

```
uint32_t SX1276GetTimeOnAir( RadioModems_t modem, uint8_t pktLen )
```
Computes the packet time on air in ms for the given payload.

#### Arguments
| name | type | description |
| --- | --- | --- |
| modem | RadioModems_t | Radio modem to be used [0: FSK, 1: LoRa] |
| pktLen| uint8_t | Packet payload length |

#### Return
| return | description |
| --- | --- |
| uint32_t | Computed airTime (ms) for the given packet payload length |

### SX1276Send

```
void SX1276Send( uint8_t *buffer, uint8_t size )
```
Sends the buffer of size. Prepares the packet to be sent and sets the radio in transmission.

#### Arguments
| name | type | description |
| --- | --- | --- |
| buffer | uint8_t * | Buffer pointer |
| size | uint8_t | Buffer size |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276SetSleep

```
void SX1276SetSleep( void )
```
Sets the radio in sleep mode.

#### Arguments
| name | type | description |
| --- | --- | --- |
| void | void | NA |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276SetStby

```
void SX1276SetStby( void )
```
Sets the radio in standby mode.

#### Arguments
| name | type | description |
| --- | --- | --- |
| void | void | NA |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276SetRx

```
void SX1276SetRx( uint32_t timeout )
```
Sets the radio in reception mode for the given time.

#### Arguments
| name | type | description |
| --- | --- | --- |
| timeout | uint32_t | Reception timeout [ms] [0: continuous, others timeout] |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276StartCad

```
void SX1276StartCad( void )
```
Start a Channel Activity Detection.

#### Arguments
| name | type | description |
| --- | --- | --- |
| void | void | NA |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276SetTxContinuousWave

```
void SX1276SetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time )
```
Sets the radio in continuous wave transmission mode.

#### Arguments
| name | type | description |
| --- | --- | --- |
| freq | uint32_t | Channel RF frequency |
| power | int8_t | Sets the output power [dBm] |
| time | uint16_t | Transmission mode timeout [s] |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276ReadRssi

```
int16_t SX1276ReadRssi( RadioModems_t modem )
```
Reads the current RSSI value.

#### Arguments
| name | type | description |
| --- | --- | --- |
| modem | RadioModems_t | Radio modem to be used [0: FSK, 1: LoRa] |

#### Return
| return | description |
| --- | --- |
| int16_t | Current RSSI value in [dBm] |

### SX1276Write

```
int16_t SX1276ReadRssi( RadioModems_t modem )
```
Writes the radio register at the specified address.

#### Arguments
| name | type | description |
| --- | --- | --- |
| addr | uint16_t | addr Register address |
| data | uint8_t | data New register value |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276Read

```
uint8_t SX1276Read( uint16_t addr )
```
Reads the radio register at the specified address.

#### Arguments
| name | type | description |
| --- | --- | --- |
| addr | uint16_t | addr Register address |

#### Return
| return | description |
| --- | --- |
| uint8_t | Register value |

### SX1276WriteBuffer

```
void SX1276WriteBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
```
Writes multiple radio registers starting at address.

#### Arguments
| name | type | description |
| --- | --- | --- |
| addr | uint16_t | First Radio register address |
| buffer | uint8_t * | Buffer containing the new register's values |
| size | uint8_t | Number of registers to be written |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276ReadBuffer

```
void SX1276ReadBuffer( uint16_t addr, uint8_t *buffer, uint8_t size )
```
Reads multiple radio registers starting at address.

#### Arguments
| name | type | description |
| --- | --- | --- |
| addr | uint16_t | First Radio register address |
| buffer | uint8_t * | Buffer where to copy the registers data |
| size | uint8_t | Number of registers to be written |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276SetMaxPayloadLength

```
void SX1276SetMaxPayloadLength( RadioModems_t modem, uint8_t max )
```
Sets the maximum payload length.

#### Arguments
| name | type | description |
| --- | --- | --- |
| modem | RadioModems_t | Radio modem to be used [0: FSK, 1: LoRa] |
| max | uint8_t | Maximum payload length in bytes |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276SetPublicNetwork

```
void SX1276SetPublicNetwork( bool enable )
```
Sets the network to public or private. Updates the sync byte.

#### Arguments
| name | type | description |
| --- | --- | --- |
| enable | bool | if true, it enables a public network |

#### Return
| return | description |
| --- | --- |
| void | void |

### SX1276GetWakeupTime

```
uint32_t SX1276GetWakeupTime( void )
```
Gets the time required for the board plus radio to get out of sleep.[ms]

#### Arguments
| name | type | description |
| --- | --- | --- |
| void | void |  |

#### Return
| return | description |
| --- | --- |
| uint32_t | Radio plus board wakeup time in ms |

## Reference

https://github.com/Lora-net/LoRaMac-node



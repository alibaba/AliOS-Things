README.md: lorawan

## Contents

lorawan_4_4_2/
├── commissioning.h
├── linkwan
│   ├── include
│   │   ├── linkwan_at.h
│   │   ├── linkwan_config.h
│   │   ├── linkwan.h
│   │   └── linkwan_ica_at.h
│   ├── linkwan_at.c
│   ├── linkwan.c
│   ├── linkwan_ica_at.c
│   └── region
│       ├── RegionCN470A.c
│       └── RegionCN470A.h
├── lora
│   ├── mac
│   │   ├── LoRaMacAdr.c
│   │   ├── LoRaMacAdr.h
│   │   ├── LoRaMac.c
│   │   ├── LoRaMacClassB.c
│   │   ├── LoRaMacClassBConfig.h
│   │   ├── LoRaMacClassB.h
│   │   ├── LoRaMacCommands.c
│   │   ├── LoRaMacCommands.h
│   │   ├── LoRaMacConfirmQueue.c
│   │   ├── LoRaMacConfirmQueue.h
│   │   ├── LoRaMacCrypto.c
│   │   ├── LoRaMacCrypto.h
│   │   ├── LoRaMacFCntHandler.c
│   │   ├── LoRaMacFCntHandler.h
│   │   ├── LoRaMac.h
│   │   ├── LoRaMacHeaderTypes.h
│   │   ├── LoRaMacMessageTypes.h
│   │   ├── LoRaMacParser.c
│   │   ├── LoRaMacParser.h
│   │   ├── LoRaMacSerializer.c
│   │   ├── LoRaMacSerializer.h
│   │   ├── LoRaMacTest.h
│   │   ├── LoRaMacTypes.h
│   │   ├── region
│   │   │   ├── Region.c
│   │   │   ├── RegionCommon.c
│   │   │   ├── RegionCommon.h
│   │   │   ├── Region.h
│   │   └── secure-element.h
│   ├── radio
│   │   └── radio.h
│   └── system
│       ├── crypto
│       │   ├── aes.c
│       │   ├── aes.h
│       │   ├── cmac.c
│       │   ├── cmac.h
│       │   └── soft-se.c
│       ├── delay.c
│       ├── delay.h
│       ├── gpio.c
│       ├── gpio.h
│       ├── lorawan_port.h
│       ├── low_power.c
│       ├── low_power.h
│       ├── precision.h
│       ├── spi.h
│       ├── systime.c
│       ├── systime.h
│       ├── timer.h
│       ├── timeServer.c
│       ├── timeServer.h
│       ├── utilities.c
│       ├── utilities.h
│       └── version.h
│   └── ucube.py
├── aos.mk
└── README.md

## Introduction

This is LoRaWAN component, 

### Features

* Class A, is the base feature that all the LoRaWAN Node must provide
* Class B, is an optional feature supported by some LoRaWAN Node, can be open or closed by MACRO "LORAMAC_CLASSB_ENABLED"
* Class C, continuous reception feature in the scene that no need to consider power consumption

### Dependencies

* kv, to storage some Non-Volatile information, such as DEVEUI, JOINEUI, APPKEY

## API

### LoRaMacMlmeRequest
```
LoRaMacStatus_t LoRaMacMlmeRequest( MlmeReq_t* mlmeRequest )
```
This API is called when LoRaWAN App need to send MAC Command to LoRaWAN Server.

#### Arguments
| name | type | description |
| --- | --- | --- |
| mlmeRequest | MlmeReq_t | |

#### Return
| return | description |
| --- | --- |
| LORAMAC_STATUS_OK | success |
| Others | fail |

### LoRaMacMcpsRequest
```
LoRaMacStatus_t LoRaMacMcpsRequest( McpsReq_t* mcpsRequest )
```
This API is called when LoRaWAN Node need to send confirmed or unconfirmed data to LoRaWAN Server.

#### Arguments
| name | type | descriptions |
| --- | --- | --- |
| mcpsRequest | McpsReq_t | |

#### Return
| return | description |
| --- | --- |
| LORAMAC_STATUS_OK | success |
| Others | fail |

## Porting
When porting LoRaWAN Stack to a new board, interfaces listed as follows must be realized.

### aos_lrwan_chg_mode
| name | descriptions |
| --- | --- |
| enter_stop_mode | to enter stop mode when no work to do |
| exit_stop_mode |  to exit stop mode when need |
| enter_sleep_mode | to enter sleep mode when enter low power mode |

### Timer Interface
| name | descriptions |
| --- | --- |
| RtcDelayMs | delay interface, unit is micro second |
| RtcSetTimerContext | record the global RTC counter |
| RtcGetTimerContext | get the recorded global RTC counter |
| RtcGetTimerElapsedTime | get the value of RTC counter has elapsed from last time |
| RtcStopAlarm | stop the alarm |
| RtcSetAlarm | set the alarm |
| RtcSetMcuWakeUpTime | calculate the wake up time between wake up and mcu start |
| RtcGetTimerValue | get the current time |
| RtcTempCompensation | get the temperature compensation for RTC |

### GPIO Interfaces
| name | descriptions |
| --- | --- |
| GpioMcuInit | Init GPIOs |
| GpioMcuSetInterrupt | Set GPIO interrupt properties |
| GpioMcuRemoveInterrupt | Remove GPIO interrupt properties |
| GpioMcuWrite | Write GPIO pin data |
| GpioMcuRead | Read GPIO pin data |

### SPI Interfaces
| name | descriptions |
| --- | --- |
| SpiInit | Init the SPI |
| SpiDeInit | Deinit the SPI |
| SpiFormat | Set SPI properties |
| SpiFrequency | Set SPI frequency |
| SpiInOut | Read and Write SPI data |

## Reference

https://github.com/Lora-net/LoRaMac-node



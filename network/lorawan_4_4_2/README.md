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
├── lorachip
│   ├── lorachip.mk
│   ├── sx1276
│   │   ├── sx1276.c
│   │   ├── sx1276.h
│   │   ├── sx1276Regs-Fsk.h
│   │   └── sx1276Regs-LoRa.h
│   └── ucube.py
├── lorawan_4_4_2.mk
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

### aos_lrwan_time_itf
| name | descriptions |
| --- | --- |
| enter_stop_mode | to enter stop mode when no work to do |
| exit_stop_mode |  to exit stop mode when need |
| enter_sleep_mode | to enter sleep mode when enter low power mode |

### aos_lrwan_time_itf
| name | descriptions |
| --- | --- |
| delay_ms | delay interface, unit is micro second |
| set_timer_context | record the global RTC counter |
| get_timer_context | get the recorded global RTC counter |
| get_delta_context | calculate the difference of the two counters |
| get_timer_elapsed_time | get the value of RTC counter has elapsed from last time |
| stop_alarm | stop the alarm |
| set_alarm | set the alarm |
| set_uc_wakeup_time | calculate the wake up time between wake up and mcu start |
| set_timeout | calculate the alarm |
| compute_elapsed_time | calculate the time elapsed |
| get_current_time | get the current time |
| set_timer_val | set RTC timer value |
| get_temp_compensation | get the temperature compensation for RTC |

### aos_lrwan_radio_ctrl
| name | descriptions |
| --- | --- |
|radio_reset | reset radio by gpio |
| radio_reset_cfg_input | configure radio reset pin as input |
| radio_rw_en | enable radio data tx/rx |
| radio_rw_dis | disable radio data tx/rx |
| radio_rw | write and read radio data |

## Reference

https://github.com/Lora-net/LoRaMac-node



## Content

```sh
canopen
├── aos.mk
├── Config.in
├── adapter
│   ├── co_adapter_can.c
│   ├── co_adapter_timer.c
│   └── co_adapter.h
├── api
│   ├── canopen.c
│   └── canopenapi.h
├── README.md
└── ucube.py
```


## Introduction
* CANopen is a CAN application layer protocol, Like other application layers, CANopen is a low-level industrial application layer    protocol for automation applications. CANopen connects automation devices together using peer messaging. Built on the standard     CAN physical communications standard.
* CANopen protocol library canfestival version is 3.0
* CANopen protocol implementation based on Cortex-M4 platform .
* this library has varified under on stm32f429zi-nucleo evaluation board.

## Features
* Multi-master Slave Communication system
* Data rate:125K, 250K, 500K, 1M
* Max Number Devices:127
* Object Dictionary describes the complete functionality of a device.
* SYNC Messages, NMT Messages, EMERGENCY Messages, HEARTBEAT Messages, PDO, SDO.

## Requirements
Depends on open sourced library canfestival :
the library address  is : [https://bitbucket.org/Mongo/canfestival-3-asc/overview](https://bitbucket.org/Mongo/canfestival-3-asc/overview)

**copy list of the files from library  to network/canopen directory**

**canfestival/include directory to canopen/include directory(total 23 files)**

```c
can_driver.h,
can.h,
data.h,
dcf.h,
def.h,
emcy.h,
lifegrd.h,
lss.h,
nmtMaster.h,
nmtSlave.h,
objacces.h,
objdictdef.h,
pdo.h,
sdo.h,
states.h,
sync.h,
sysdep.h,
timer.h,
timers_driver.h,
cm4/applicfg.h,
cm4/timerscfg.h,
cm4/canfestival.h,
AVR/config.h
```

**canfestival/src directory to canopen/src directory(total 12 files)** :

```
dcf.c,
emcy.c,
lifegrd.c,
lss.c,
nmtMaster.c,
nmtSlave.c,
objacces.c,
pdo.c,
sdo.c,
states.c,
sync.c,
timer.c
```

**modify some code in config.h and timer.c file**

**open config.h file and delete list of codes**

```
#ifdef  __IAR_SYSTEMS_ICC__
#include <ioavr.h>
#include <intrinsics.h>
#include "iar.h"
#else    // GCC
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#endif    // GCC

//#define WD_SLEEP
// Needed defines by Atmel lib
#define FOSC           8000        // 8 MHz External cristal
#ifndef F_CPU
#define F_CPU          (1000UL*FOSC) // Need for AVR GCC
#endif
#define CAN_BAUDRATE    125
```

**modify timer.c file**
```
#include "timer.h"  modified to  #include "../include/timer.h"
```

## API

User service APIs:

```
/*CANopen export service APIs*/
int canopen_init(PORT_CAN_TYPE can_port, PORT_TIMER_TYPE timer_port, uint32_t baud, CO_Data* d);
int canopen_od_init(CO_Data* d, co_register_function_callback_t* fun_callback);
int canopen_od_handler(CO_Data* d, uint16_t index, uint8_t sub_index, co_objectdict_cb* od_callback);
int init_nodes(CO_Data* d, uint8_t id);
```

## Application

    you can write your own app and object dictionary to use this library.


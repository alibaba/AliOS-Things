## Contents

```sh
ULOG
├── aos.mk
├── Config.in
├── include
│   ├── ulog_config.h
│   └── uring_fifo.h
├── README.md
├── ucube.py
├── ulog.c
└── uring_fifo.c
```

## Introduction

ulog is an asyncronized log mechanism which keeps the other logical task work in their order, based on syslog protocol, log content can pop out with difference sessions(uart, udp, file system, usb).

## Features

1. syslog format or pure content pop out, user can select it via EXTREAM_LOG_TEXT,SYSLOG_SIZE in ulog_config.h;
2. depth of fifo which record the log content ready for pop out can be select via initialization;
3. dynamically change the stop filter of log level, select the pop out session;
4. Firmware digital signature verification.
5. resource cost: ROM-->1.5K RAM: 3K~9K(0.2K static data, 0.5K task depth, 2K~8K fifo depth, user can modify it via initialization & SYSLOG_SIZE)

## Dependencies

None, this feature has varified under stm32f4, stm32l4, esp8266, mk3060.
modify log parameter via CLI depends on module cli.

## API

User service APIs:

```c
/*ULOG export service APIs*/
void ulog_init(const uint8_t host_name[8], const uint16_t depth);
ULOG(M, S, F, L, ...);
ULOGOS(S, ...);```

## Simple use it
Step 1. Depend middleware.uagent.ulog in your program, e.g. aos.mk in helloworld
Step 2. Include ulog declaration header aos/ulog.h
Step 3. Initialize ulog function via  ulog_init("SOC", 64), parameter fill as you wish, 2nd parameter 64 is recommended, depends on your log frequncy and system resouce
Step 4. ULOGOS(LOG_ERR, "error happens, param %d", param) to log your action.

## Other

syslog format:
<131>Jan 01 00:00:01.180 SOC OS_helloworld.c[13]: helloworld
PRI  |HEADER                 |MSG[TAG+Content]
more detail please refer as BSD syslog.

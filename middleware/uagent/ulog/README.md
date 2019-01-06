## Contents

```sh
ulog
├── aos.mk
├── Config.in
├── include
│   ├── log_impl.h
│   ├── ulog_config.h
│   └── uring_fifo.h
├── log.c
├── README.md
├── session_udp.c
├── ucube.py
├── ulog.c
├── ulog_init.c
└── uring_fifo.c
```

## Introduction

ulog is log module of AliOS Things, support both classic syncronized mechanism and an asyncronized log mechanism which keeps the other logical task work in their order, based on syslog protocol, log content can pop out with difference sessions(uart, udp, file system, usb).

## Features

0. classic syncronized is default used, which comes from former utility/log;
1. syslog format or pure content pop out, user can select it via EXTREAM_LOG_TEXT,SYSLOG_SIZE in ulog_config.h;
2. depth of fifo which record the log content ready for pop out can be select via initialization;
3. dynamically change the stop filter of log level, select the pop out session;
4. Firmware digital signature verification.
5. resource cost: ROM-->0.5K~2.2K(0.5K for classic sync mode) RAM: 3K~9K(0.2K static data, 0.5K task depth, 2K~8K fifo depth, user can modify it via SYSLOG_SIZE)

## Dependencies

None, this feature has varified under stm32f4, stm32l4, esp8266, mk3060(pop out log via udp has verified in soc).
modify log parameter via CLI depends on module cli.
pop out log via udp via tcpip related(native tcpip stack, lwip, or sal)

## API

User service APIs:

```c
/*ULOG export service APIs*/
void ulog_init(const uint8_t host_name[8]);
void ulog_man(const char* cmd_str);
ULOG(M, S, F, L, ...);
ULOGOS(S, ...);
```

## Simple use it
Step 1. Depend ulog in your program, e.g. aos.mk in helloworld. Actually, the module kernel/init has already depends on ulog as default.
Step 2. Include ulog declaration header ulog/ulog.h
Step 3. (Auto) Initialize ulog function via ulog_init("SOC"),(this step is already performed in init/aos_init.c) parameter fill as you wish, which will be recorded as part of HEADER.
Step 4. ULOGOS(LOG_ERR, "error happens, param %d", param) to log your action.
Step 5. Add compiler argument "log_async=1" to build the project, e.g. "aos make helloworld@esp8266 ULOG_CONFIG_ASYNC=y".
Step 6. pop log out via udp: You shall notify the ulog the tcpip is ready via "ulog_man("tcpip on=1");" e.g. in wifi_service_event when got ip. You shall specify the syslog watcher's address via "ulog_man("listen ip=192.168.1.100");" or via CLI(ulog a 192.168.1.100).
REMARK: If the OS crash once the ulog introduced, maybe because the LOG_ROUTINE_TASK_STACK_DEPTH shall be adjusted, default value 512 is fit in most board, except MK3060 for now. So adjust this value to 1024 if you use ulog in MK3060.

## Other

syslog format:
<131>Jan 01 00:00:01.180 SOC OS_helloworld.c[13]: helloworld
PRI  |HEADER                 |MSG[TAG+Content]
more detail please refer as BSD syslog.

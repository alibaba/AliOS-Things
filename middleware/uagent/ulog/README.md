## Contents

```sh
ulog
├── aos.mk
├── Config.in
├── include
│   ├── log_impl.h
│   ├── ulog_api.h
│   ├── ulog_config.h
│   └── uring_fifo.h
├── log.c
├── README.md
├── session_fs.c
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
5. resource cost: ROM-->4K(0.5K for classic sync mode) RAM: 3K~9K(0.2K static data, 1K~3K task depth, 2K~6K fifo depth.
   user can modify task depth or fifo depth via ULOG_CONFIG_LOG_ROUTINE_TASK_STACK_DEPTH, ULOG_CONFIG_ASYNC_BUF_SIZE)

## Dependencies

None, this feature has varified under stm32f412ZG-Nucleo, developerkit, esp8266, mk3060, esp32devkitc(pop out log via udp has verified in soc).
modify log parameter via CLI depends on module cli.
pop out log via udp via tcpip related(native tcpip stack, lwip, or sal)

## API

User service APIs:

```c
/*ULOG export service APIs*/
void ulog_init(const uint8_t host_name[8]);
void ulog_man(const char* cmd_str);
ulog(M, S, F, L, ...);
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
This function only support on SoC for now.
Step 7. pop log out via local file system. If component vfs and spiffs are builded in, then the pop out via local file system supported. ulog000.log ~ ulogXXX.log may produced under this mechanism.
ulog000.log is used in save the log cfg, currently only log file index saved in it which help record up-most log file index in file system, so the new log will saved in this index, or new one after system boot-up.
ulog001.log ~ ulog_XXX.log will be produced, the XXX is config item ULOG_CONFIG_LOCAL_FILE_CNT, each log size is not larger than ULOG_CONFIG_LOCAL_FILE_SIZE+ULOG_CONFIG_SYSLOG_SIZE.
Rolling back mechanism also used to make new file: if ulogXXX is full, than the new log context will be recorded in ulog001.log.

## Other

syslog format:
<131>Jan 01 00:00:01.180 SOC helloworld.c[13]: helloworld
PRI  |HEADER                 |MSG[TAG+Content]
more detail please refer as BSD syslog.

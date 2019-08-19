## Contents

```sh
ulog
├── aos.mk
├── Config.in
├── include
│   └── ulog_config.h
├── README.md
├── ucube.py
├── ulog_api.h
├── ulog_async.c
├── ulog.c
├── ulog_fs_cfg.c
├── ulog_init.c
├── ulog_ring_fifo.c
├── ulog_ring_fifo.h
├── ulog_session_file.c
├── ulog_session_file.h
├── ulog_session_udp.c
└── ulog_utility.c
```

## Introduction

ulog is log module of AliOS Things, support both classic syncronized mechanism and an asyncronized log mechanism which keeps the other logical task work in their order, based on brief log format and syslog protocol format, log content can pop out with difference sessions(uart, udp, file system, via uagent). Besides, log files in file system can post via http.

## Features

1. Classic log on uart(terminal for linuxhost) is default on;
2. You can switch on more information as you want by 'aos make menuconfig'.
3. User can switch on 'Log to Cloud', 'Log via syslog udp', 'Log into Filesystem' via 'aos make menuconfig'.
4. depth of fifo which record the log content ready for pop out can be select via initialization;
5. dynamically change the stop filter of log level, select the pop out session;
6. resource cost: ROM-->4K(0.5K for classic sync mode) RAM: 3K~9K(0.2K static data, 1K~3K task depth, 2K~6K fifo depth.
   user can modify task depth or fifo depth via ULOG_CONFIG_LOG_ROUTINE_TASK_STACK_DEPTH, ULOG_CONFIG_ASYNC_BUF_SIZE)

## Dependencies

vfs, and one of spiffs or fatfs, if user want to save log files in file system.
uagent, if user want to post log real-time, or upload logs saved in file system.

## API

User service APIs:

```c
/*ULOG export service APIs*/
void ulog_init(void);

LOG(), LOGF(), LOGE(), LOGW(), LOGI(), LOGD();

int aos_get_ulog_list(char *buf, const unsigned short len);

int ulog_man(const char *cmd_str);

int ulog(const unsigned char s, const char *mod, const char *f,
         const unsigned long l, const char *fmt, ...);

```

## Simple use it
Step 1. Depend ulog in your program, e.g. aos.mk in helloworld. Actually, the module kernel/init has already depends on ulog as default.
Step 2. Include ulog declaration header ulog/ulog.h
Step 3. (Auto) Initialize ulog function via ulog_init(),(this step is already performed in init/aos_init.c).
Step 4. Calling LOGX() to log your action. Usually LOGI to record some key text, LOGW to record warning or key information may invoke error, LOGE to record error. LOGD can record some debug information, notice this level log usually forbidden in release version as it usually only cared by developer of customer module. LOGF recored the fatal information may impact on whole system, LOG is not recommeded using.
Step 5. pop log out via udp: You shall notify the ulog the tcpip is ready via "ulog_man("tcpip on=1");" e.g. in wifi_service_event when got ip. You shall specify the syslog watcher's address via "ulog_man("listen ip=192.168.1.100");" or via CLI(ulog a 192.168.1.100).
Step 6. Pop log out via local file system depends on component vfs and spiffs are builded in, then the pop out via local file system supported. ulog000.log ~ ulogXXX.log may produced under this mechanism.
ulog000.log is used in save the log cfg, currently only log file index saved in it which help record up-most log file index in file system, so the new log will saved in this index, or new one after system boot-up.
ulog001.log ~ ulog_XXX.log will be produced, the XXX is config item ULOG_CONFIG_LOCAL_FILE_CNT, each log size is not larger than ULOG_CONFIG_LOCAL_FILE_SIZE+ULOG_CONFIG_SYSLOG_SIZE.
Rolling back mechanism also used to make new file: if ulogXXX is full, than the new log context will be recorded in ulog001.log.
Step 7. uagent module will be selected if user want to pop out log to cloud, or upload log file via http. In this case, smart device maintain kit, or simple tool Aliyun IoT Console is needed.

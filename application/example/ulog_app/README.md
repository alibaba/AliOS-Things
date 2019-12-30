## Contents

```sh
ulog_app
├── aos.mk
├── Config.in
├── k_app_config.h
├── README.md
└── ulog_app.c
```

## Introduction

The **ulog_app** example show the demo of ulog.

### Dependencies

* ulog
* cli if try to test modify the log parameter dynamically via command line
* netmgr if try to test log pop out via udp, or test get log files located in file system out from tftp
* vfs and spiffs if try to test log pop out via filesystem
* fatfs if try to test get log files located in file system into removeable SD card

### Build

```sh
aos make clean
aos make ulog_app@yourboard ULOG_CONFIG_ASYNC=y
aos make ulog_app@yourboard ULOG_CONFIG_ASYNC=y FEATURE_TFTP_ENABLED=y use_lwip_tftp=1 FEATURE_TFTP_FS=y
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

N.A.

### Result
N.A.

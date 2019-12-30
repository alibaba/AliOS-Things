## Contents

## Introduction
**uLocation** provides locating service based on wifi / 2g GPRS signals.

There are mainly 3 layers in uLocation framework, on the bottom is hardware adapting layer which mainly interract with wifi/gsm/gps module to get locating information; On the middle is main logic handling of uLocation while on the topest is a layer interconnect with cloud while Gaode locating service locates.

### Features
- wifi locating
- GPRS cell id locating
- gps
- qian xun wei zhi difference SDK

### Directories

```sh
└── ulocation
    ├── aos.mk
    ├── Config.in
    ├── hal
    │   ├── ulocation_hal_gprs.c
    │   ├── ulocation_hal_gps.c
    │   ├── ulocation_hal.h
    │   └── ulocation_hal_wifi.c
    ├── qianxun
    │   ├── include
    │   ├── lib
    |   ├── ulocation_qxwz_service.c
    |   ├── ulocation_qxwz_socket.c
    │   └── ulocation_qxwz_utils.c
    ├── include
    ├── ulocation_common.h
    ├── ulocation_main.c
    ├── ulocation_north.c
    └── ulocation_south.c



```

### Dependencies


## Reference


## Contents

## Introduction
**uLocation** provides locating service based on wifi / 2g GPRS signals.

There are mainly 3 layers in uLocation framework, on the bottom is hardware adapting layer which mainly interract with wifi/gsm/gps module to get locating information; On the middle is main logic handling of uLocation while on the topest is a layer interconnect with cloud while Gaode locating service locates.

### Features
- wifi locating
- GPRS cell id locating
- gps

### Directories

```sh
└── ulocation
    ├── aos.mk
    ├── Config.in
    ├── hal
    │   ├── uloc_hal_gprs.c
    │   ├── uloc_hal_gps.c
    │   ├── uloc_hal.h
    │   └── uloc_hal_wifi.c
    ├── include
    ├── uloc_common.h
    ├── uloc_main.c
    ├── uloc_north.c
    └── uloc_south.c


```

### Dependencies


## Reference


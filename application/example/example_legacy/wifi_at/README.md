## Contents

```shell
├── wifi_at.c
├── atcmd_util.c
├── Config.in
├── aos.mk
├── README.md
└── include
      ├── atcmd_config.h
      └── atcmd_util.h
```

## Introduction

`wifi_at` is an demostration for how to use Aliyun WiFi AT module to connect Aliyun IoT platform. Specifcially, this example sets up a connection, post property to platform and handle service set request. 


Compile command example:

```sh
# generate wifi_at@stm32f103rb-nucleo default config
aos make wifi_at@stm32f103rb-nucleo -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Features

- **WIFI connection**.  `wifi_at` provides two ways to connect wifi AP, use AWSS of WIFI module or direct AT command.
- **WIFI module operation**.  `wifi_at` provides sample of property post and anwser service set requst.
- **scalability**. `wifi_at` provides a tool `atcmd_util` to add more AT command operateion.

### Dependencies

- at

### Supported Boards

- stm32xxx

## Reference

None.

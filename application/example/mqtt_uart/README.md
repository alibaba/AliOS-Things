## Contents

```sh
.
├── aos.mk
├── app_entry.c
├── app_entry.h
├── Config.in
├── k_app_config.h
├── mqtt_uart.c
└── README.md
```

## Introduction

本应用可以对接各种uart接口的外部设备，将数据解析工作放到了阿里云IOT平台上。
代码中实现了mqtt和uart协议之间的转换，可以实现设备uart和阿里云IOT平台之间的数据透传。
当设备uart上收到数据后，通过mqtt协议将数据上传到阿里云IOT平台，用于物模型数据解析。
当设备接收到阿里云IOT平台通过mqtt协议下发的数据后，通过uart输出给外部设备。

### Dependencies

* yloop
* cli
* linkkit

### Supported Boards

- haas100
- mk3060
- mk3080
- esp8266
- esp32devkitc
- uno-91h
- bk7231
- stm32f429zi-nucleo

### Build

```sh

aos make mqtt_uart@haas100 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

1).mqtt_uart.c (default):

```sh
    aos make
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).


## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)
* [gateway](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/MQTT_Connect)
* [物模型数据解析](https://help.aliyun.com/document_detail/68702.html?spm=a2c4g.11186623.6.699.497c678aO5ZCmG)

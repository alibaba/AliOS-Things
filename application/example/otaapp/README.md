## Contents

```sh
otaapp demo
├── aos.mk
├── Config.in
├── k_app_config.h
├── otaapp.c
├── README.md

```

## Introduction

An over-the-air update is the wireless delivery of new software or data to smart devices, especially IoT devices. Wireless carriers and OEMs typically use over-the-air (OTA) updates to deploy the new operating systems and the software app to these devices.

## Features

1. Differential incremental upgrade;
2. Dual banker:AB partition upgrade to support rollback to old version;
3. Secure download channel;
4. Firmware digital signature verification.

## Dependencies

Linkkit MQTT channel
Linkkit CoAP channel

### Supported Boards

- mk3060
- mk3080
- developerkit
- esp8266

## API

User service APIs:

```c
include/ota/
├── ota_agent.h
├── ota_hal.h
└── ota_updater.h
```
for sample code please check [otaapp](../../../app/example/otaapp/).
## RTOS build

```sh
# generate otaapp@esp8266 default config
aos make otaapp@esp8266 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

## run CLI CMDs
1. connect network
```
netmgr connect ssid passwd
```
2. run ota demo

```
OTA_APP pk dn ds ps
```

## Reference

* https://github.com/alibaba/AliOS-Things/wiki/OTA-Tutorial
* https://github.com/alibaba/AliOS-Things/wiki/OTA-Flash-Partitions-Overview
* https://mp.weixin.qq.com/s/Pb8Lleuww1r7qQJHu5ON8g

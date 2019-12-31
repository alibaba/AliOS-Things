## Contents

```sh
OTA
├── aos.mk
├── Config.in
├── hal
│   ├── ota_hal_ctrl.c
│   ├── ota_hal_ctrl.h
│   ├── ota_hal_digest.c
│   ├── ota_hal_digest.h
│   ├── ota_hal_os.c
│   ├── ota_hal_os.h
│   ├── ota_hal_param.c
│   ├── ota_hal_param.h
│   ├── ota_hal_plat.c
│   ├── ota_hal_trans.c
│   └── ota_hal_trans.h
├── include
│   ├── ota_ble.h
│   ├── ota_import.h
│   ├── ota_log.h
├── agent
│   ├── download
│   │   ├── ota_download_ble.c
│   │   ├── ota_download_coap.c
│   │   └── ota_download_http.c
│   ├── mcu
│   │   ├── ota_mcu_can.c
│   │   ├── ota_mcu_can_slave.c
│   │   ├── ota_mcu_upgrade.c
│   │   ├── ota_mcu_upgrade.h
│   │   └── ota_mcu_ymodem.c
│   ├── ota_service.c
│   ├── transport
│   │   ├── ota_transport_ble.c
│   │   ├── ota_transport_coap.c
│   │   └── ota_transport_mqtt.c
│   └── verify
│       ├── ota_public_key.h
│       ├── ota_verify_hash.c
│       └── ota_verify_rsa.c
└── README.md

```

## Introduction

An over-the-air update is the wireless delivery of new software or data to smart devices, especially IoT devices. Wireless carriers and OEMs typically use over-the-air (OTA) updates to deploy the new operating systems and the software app to these devices.

## Features

1. Differential incremental upgrade;
2. Dual banker:AB partition upgrade to support rollback to old version;
3. Secure download channel;
4. Firmware digital signature verification.
5. footprint: ROM-->12K~18K RAM: 3K~8K

## Dependencies

Linkkit MQTT channel
Linkkit CoAP channel

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
cd ROOT DIR;
aos make otaapp@board;
```

## run CLI CMDs
1. connect network
```
netmgr connect ssid passwd
```
2. burn to run ota demo

```

## Reference

* https://github.com/alibaba/AliOS-Things/wiki/OTA-Tutorial
* https://github.com/alibaba/AliOS-Things/wiki/OTA-Flash-Partitions-Overview
* https://mp.weixin.qq.com/s/Pb8Lleuww1r7qQJHu5ON8g

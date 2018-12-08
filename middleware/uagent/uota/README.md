## Contents

```sh
OTA
├── hal
├── ota_service.c
├── src
│   ├── 2nd_boot
│   ├── device
│   ├── download
│   ├── transport
│   └── verify
└── aos.mk
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
/*OTA export service APIs*/
int ota_service_init(ota_service_t* ctx);
int ota_service_deinit(ota_service_t* ctx);
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
2. run ota demo

```
OTA_APP pk dn ds ps
```

## Reference

* [AliOS-Things OTA使用说明](https://github.com/alibaba/AliOS-Things/wiki/OTA-Tutorial)
* [OTA flash分区说明文档](https://github.com/alibaba/AliOS-Things/wiki/OTA-Flash-Partitions-Overview)
* [云端一体化差分+安全升级，AliOS Things物联网升级“利器”](https://mp.weixin.qq.com/s/Pb8Lleuww1r7qQJHu5ON8g)

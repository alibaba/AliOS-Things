## Contents

```sh
xz
├── aos.mk
├── Config.in
├── linux
│   ├── include
│   │   └── linux
│   │       ├── decompress
│   │       │   └── unxz.h
│   │       └── xz.h
│   └── lib
│       ├── decompress_unxz.c
│       └── xz
│           ├── xz_config.h
│           ├── xz_crc32.c
│           ├── xz_crc64.c
│           ├── xz_dec_bcj.c
│           ├── xz_dec_lzma2.c
│           ├── xz_dec_stream.c
│           ├── xz_lzma2.h
│           ├── xz_private.h
│           └── xz_stream.h
└── README.md
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

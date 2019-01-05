## Contents

```sh
ota_core
├── ota_service.c
├── src
│   ├── download
│   ├── transport
│   └── verify
└── aos.mk
```

## Introduction
OTA core is an core component for transport, download, verify.

## Features
1. Transport protcol support MQTT&CoAP;
2. Download protcol support HTTP(s)&CoAP&ITLS;
3. Verify algorithm support RSA&SHA256&MD5;
5. footprint: ROM-->6K~10K RAM: 2K~8K

## Dependencies
Linkkit MQTT channel
Linkkit CoAP channel

## API
User service APIs:

```c
/*OTA export service APIs*/
int ota_service_init(ota_service_t* ctx);
int ota_service_deinit(ota_service_t* ctx);

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

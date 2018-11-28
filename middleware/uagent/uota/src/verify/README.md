## Contents

```sh
verify
├── base64
│   ├── ota_base64.c
│   └── ota_base64.h
├── Config.in
├── crc
│   ├── ota_crc.c
│   └── ota_crc.h
├── hash
│   ├── ota_hash.c
│   ├── ota_hash.h
│   ├── ota_md5.c
│   ├── ota_md5.h
│   ├── ota_sha256.c
│   └── ota_sha256.h
├── ota_public_key_config.h
├── ota_rsa_param.h
├── ota_rsa_verify.c
├── ota_rsa_verify.h
├── ota_verify.c
├── ota_verify.h
├── README.md
└── rsa
    ├── ota_asn1.h
    ├── ota_bignum.c
    ├── ota_bignum.h
    ├── ota_bn_mul.h
    ├── ota_hash_rsa.c
    ├── ota_oid.h
    ├── ota_pub.h
    ├── ota_rsa.c
    └── ota_rsa.h
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

## 目录结构

```sh
OTA
├── aos.mk
├── Config.in
├── hal
│   ├── ota_hal_ctrl.c
│   ├── ota_hal_digest.c
│   ├── ota_hal_digest.h
│   ├── ota_hal_fs_ctrl.c
│   ├── ota_hal_fs_plat.c
│   ├── ota_hal_os.c
│   ├── ota_hal_os.h
│   ├── ota_hal_param.c
│   ├── ota_hal_plat.c
│   ├── ota_hal_trans.c
│   └── ota_hal_trans.h
├── include
│   ├── ota_agent.h
│   ├── ota_ble.h
│   ├── ota_hal.h
│   ├── ota_import.h
│   ├── ota_log.h
│   └── ota_updater.h
├── ota_agent
│   ├── download
│   │   ├── ota_download_ble.c
│   │   ├── ota_download_coap.c
│   │   ├── ota_download_file2fs_http.c
│   │   └── ota_download_http.c
│   ├── mcu
│   │   ├── ota_download_uart.c
│   │   ├── ota_mcu_can.c
│   │   ├── ota_mcu_can_slave.c
│   │   ├── ota_mcu_ubus.c
│   │   ├── ota_mcu_ymodem.c
│   │   └── ota_slaver_upgrade.c
│   ├── transport
│   │   ├── ota_transport_ble.c
│   │   ├── ota_transport_coap.c
│   │   └── ota_transport_mqtt.c
│   └── verify
│       ├── ota_verify_hash.c
│       └── ota_verify_rsa.c
├── ota_service.c
├── README.md
└── tools
    ├── ota_image_package.py
    ├── upack_data_file.c
    └── upack_data_file.h

```
## 简介
OTA(firmware over the air)即文件远程升级，AliOS Things OTA 支持固件升级（FOTA），资源文件升级（DOTA）以及应用程序升级(SOTA);

## 主要功能
1. 文件完整性检验(MD5/SHA256).
2. 升级文件数字签名验证.
3. 断点续传
4. 多模块升级；
5. 文件系统升级
6. 内核升级

## 主要参数
footprint: ROM-->12K~18K RAM: 3K~12K

## 主要依赖组件

Linkkit http drivers ulog

## 对外接口
请参考如下头文件：
```c
ota/include/
├── ota_agent.h
├── ota_ble.h
└── ota_updater.h
```
## 用例
请参考projects/link_sdk_demos中data_model_basic_demo.c

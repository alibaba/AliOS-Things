## Contents

```
breezeapp
.
├── breezeapp.c
└── breezeapp.mk
```

## Introduction

### Requirements

* Platform: Nordic's nRF52840/nRF52832 BLE SOCs or other BLE hardware already support AliOS things Breeze SDK.

* Quintuple: device info to access to Alibaba IoT cloud.

### Features

- Breeze transport : provide data transport over BLE link.
- Breeze authentication : provice secure access to Alibaba IoT cloud.
- Breeze auxiliary provision : provide provision ability(obtation SSID and password, etc) over BLE link.
- Breeze OTA : provide unified OTA framework over BLE link.

### Dependencies

- yloop
- breeze
- CLI

## Usage

Breezeapp demonstrate usage of breeze SDK functionalities.

### Build

aos make bluetooth.breezeapp@pca10056

### Install

On ble device, take pca10056 for example, use J-flash or other tools download the compilied binary, on mobile, use Xlinkdemo APPs provided by Alibaba.

### Run

Use APP to connect and auth BLE device, APPs offers date exchange, OTA functation, etc.

### Result

Chech the hint and result on APP.

## Reference

[蓝牙辅助WiFi配网开发说明](https://github.com/alibaba/AliOS-Things/wiki/%E8%93%9D%E7%89%99%E8%BE%85%E5%8A%A9WiFi%E9%85%8D%E7%BD%91%E5%BC%80%E5%8F%91%E8%AF%B4%E6%98%8E)
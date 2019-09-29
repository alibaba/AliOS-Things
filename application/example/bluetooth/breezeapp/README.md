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

- breeze
- CLI

### Supported Boards

- pca10040
- pca10056
- esp32devkitc

## Usage

Breezeapp demonstrate usage of breeze SDK functionalities.

### Build

```sh
# generate bluetooth.breezeapp@pca10056 default config
aos make bluetooth.breezeapp@pca10056 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Install

On ble device, take pca10056 for example, use J-flash or other tools download the compilied binary, on mobile, use Xlinkdemo APPs provided by Alibaba.

### Run

Use APP to connect and auth BLE device, APPs offers date exchange, OTA functation, etc.

### Result

Check the hint and result on APP.

## Reference


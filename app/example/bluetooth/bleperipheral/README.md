## Contents

```
bleperipheral
.
├── main.c
└── bleperipheral.mk
```

## Introduction

Ble peripheral example demonstrate usage of GATT server services, including HRS(Heart Rate Service), BAS(Battery Service), DIS(Device Information Service).

### Requirements

Platform: BLE hardware already support AliOS things BLE stack, take Nordic's nRF52832/52840 for example.

### Features

- ble GATT server

### Dependencies

- bt
- CLI
- Profile of bluetooth

### Supported Boards

- pca10040
- pca10056
- esp32devkitc

## Usage

### Build

aos make bluetooth.bleperipheral@pca10056

### Install

On ble device, take pca10056 for example, use J-flash or other tools download the compilied binary.

### Run

Use BLE APPs such as LightBlue or nRF Connect to scan and check the device, and test the functions of GATT service like READ/WRITE/NOTIFITY/INDICATE.

### Result

Check the hint and result on APP.

## Reference

None.

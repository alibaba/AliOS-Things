## Contents

```
bleadv
.
├── main.c
└── bleadv.mk
```

## Introduction

### Requirements

Platform: BLE hardware already support AliOS things BLE stack, take Nordic's nRF52832/52840 for example.

### Features

- ble advertising function

### Dependencies

- bt
- CLI

### Supported Boards

- pca10040
- pca10056
- esp32devkitc

## Usage

### Build

```sh
# generate bluetooth.bleadv@pca10056 default config
aos make bluetooth.bleadv@pca10056 -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Install

On ble device, take pca10056 for example, use J-flash or other tools download the compilied binary.

### Run

use BLE APPs such as *LightBlue* or *nRF Connect* to scan and check the advertising data

### Result

Check the hint and results on APPs.

## Reference

None.

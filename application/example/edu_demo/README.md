## Contents

```sh
.
├── Config.in
├── README.md
├── aos.mk
├── app_entry.c
├── app_entry.h
├── autobuild.json
├── build_version.h
├── common
│   └── linkkit
│       ├── linkkit_entry.c
│       └── linkkit_event.c
├── k1_apps
│   ├── aircraftBattle
│   │   ├── aircraftBattle.c
│   │   └── aircraftBattle.h
│   ├── barometer
│   │   ├── barometer.c
│   │   └── barometer.h
│   ├── compass
│   │   ├── compass.c
│   │   └── compass.h
│   ├── greedySnake
│   │   ├── greedySnake.c
│   │   └── greedySnake.h
│   ├── gyroscope
│   │   ├── gyroscope.c
│   │   └── gyroscope.h
│   ├── homepage
│   │   ├── homepage.c
│   │   ├── homepage.h
│   │   └── resource.h
│   ├── humiture
│   │   ├── humiture.c
│   │   └── humiture.h
│   ├── lightmeter
│   │   ├── lightmeter.c
│   │   └── lightmeter.h
│   ├── menu.c
│   ├── menu.h
│   ├── musicbox
│   │   ├── musicbox.c
│   │   ├── musicbox.h
│   │   └── pitches.h
│   ├── shakeshake
│   │   ├── shakeshake.c
│   │   └── shakeshake.h
│   └── systemDiag
│       ├── beep_diag
│       │   └── beep_diag.c
│       ├── button_diag
│       │   └── button_diag.c
│       ├── led_diag
│       │   ├── led_diag.c
│       │   └── led_diag.h
│       ├── oled_diag
│       │   └── oled_diag.c
│       ├── resource.h
│       ├── systemDiag.c
│       └── systemDiag.h
├── k_app_config.h
├── maintask.c
└── mfg_test
    ├── adc_test.c
    ├── audio_test.c
    ├── board_test.c
    ├── i2c_test.c
    ├── key_test.c
    ├── led_test.c
    ├── oled_test.c
    ├── pwm_test.c
    ├── sd_test.c
    ├── sensors_test.c
    ├── spi_test.c
    ├── uart_test.c
    ├── usb_test.c
    └── watchdog_test.c

```

## Introduction

The **edu_app**  shows edu related functions.

### Dependencies

default

* yloop
* cli
* linkkit
* netmgr
* cjson
* oled
* lcd
* sensor


### Supported Boards

- haasEDU k1

### Build

```sh
# generate edu_demo@haaseduk1 default config
aos make edu_demo@haaseduk1 -c config

# or customize config manually
aos make menuconfig

# build
aos make

```
1)enable mfg_test module:
```sh
    git submodule init
    git submodule update
 ```
Set `Select Case` to `enable factory test function` in menuconfig, or run this command:
Run this command:
```sh
    aos make EDK_DEMO_FACTORY_TEST_ENABLIE=y
```

2) enable linkkit module:

Set `Select Case` to `Enable linkkit` in menuconfig, or run this command:
```sh
    aos make EDU_DEMO_CONFIG_LINKKIT=y
```

3)enable OTA module:

Set `Select Case` to `Enable OTA` in menuconfig, or run this command:
```sh
    aos make ENABLE_AOS_OTA=y
```

> if you want to see AliOS-Things supports boards, click [board](../../../board).

### Install

```sh
aos upload
```

> if you are not sure is the`aos upload` command supports your board, check [aos upload](../../../build/site_scons/upload).

### Result

```sh

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)
* [gateway](https://code.aliyun.com/edward.yangx/public-docs/wikis/user-guide/linkkit/Prog_Guide/API/Linkkit_Provides)

## Contents

```sh
tinyengine_app
└── tinyengine_app.c
```

## Introduction

[TinyEngine](../../../middleware/tinyengine) is an javascript runtime running on IoT device, provides an application development framework, runtime environment, and deployment toolset for developing IoT device with the Javascript language. [TinyEngine](../../../middleware/tinyengine) designed to allow IoT device applications and developers to not pay too much attention to hardware platform differences, underlying drivers and system implementations. In detail, you don't need to pay attention to the specific cloud device access technology, you can quickly develop IoT products, and realize the ability of the final application to run across hardware platforms, truly "one code, run everywhere".

**tinyengine_app** is the [TinyEngine](../../../middleware/tinyengine) app, when **tinyengine_app** flash into device and running, you can run javascript code with [嵌入式JS工作台](https://linkdevelop.aliyun.com/deviceapp#/s), like `console.log("hello world!")`, also **tinyengine_app** will run the js file when startup if you had packed js files into spiffs.bin and flash into device.

[TinyEngine](../../../middleware/tinyengine) support many MCU and SoC, including ESP8266, ESP32, MK3060,MK3080, STM32F4xx, STM32F7xx series.


more reference website please check [Reference](#reference).

### Features

* lightweight JavaScript engine
* small footprint(RAM < 15KB, ROM < 15KB)
* console
* run js file

more TinyEngine features please check [TinyEngine Github](https://github.com/aliyun/TinyEngine).

### Dependencies

* tinyengine
* log
* yloop
* cli
* cjson
* netmgr
* linkkit
* uota
* spiffs

### Supported Boards

- developerkit
- esp32devkitc

### Build & Flash

```sh
# generate tinyengine_app@developerkit default config
aos make tinyengine_app@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make

# upload
aos upload
```

### Result

**tinyengine_app** works with [嵌入式JS工作台](https://linkdevelop.aliyun.com/deviceapp#/s):

![](https://img.alicdn.com/tfs/TB1hPA7pr2pK1RjSZFsXXaNlXXa-1920-938.jpg)

## Reference

* [TinyEngine Github](https://github.com/aliyun/TinyEngine)
* [tinyengine接入LD平台](https://linkdevelop.aliyun.com/device-doc#ebagtb.html)
* [AliOS-Things TinyEngine]([TinyEngine](../../../middleware/tinyengine))
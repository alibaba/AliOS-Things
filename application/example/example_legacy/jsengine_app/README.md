## Contents

```
jsengine_app
└── jsengine_app.c
```

## Introduction

[JSEngine](../../../middleware/jsengine) is an javascript runtime running on IoT device, provides an application development framework, runtime environment, and deployment toolset for developing IoT device with the Javascript language. [JSEngine](../../../middleware/jsengine) designed to allow IoT device applications and developers to not pay too much attention to hardware platform differences, underlying drivers and system implementations. In detail, you don't need to pay attention to the specific cloud device access technology, you can quickly develop IoT products, and realize the ability of the final application to run across hardware platforms, truly "one code, run everywhere".

**jsengine_app** is the [JSEngine](../../../middleware/jsengine) app, when **jsengine_app** flash into device and running, you can run javascript code with [Embeded JS WorkStation](https://linkdevelop.aliyun.com/deviceapp#/s), like `console.log("hello world!")`, also **jsengine_app** will run the js file when startup if you had packed js files into spiffs.bin and flash into device.

[JSEngine](../../../middleware/jsengine) support many MCU and SoC, including ESP8266, ESP32, MK3060,MK3080, STM32F4xx, STM32F7xx series.


more reference website please check [Reference](#reference).

### Features

* lightweight JavaScript engine
* small footprint(RAM < 15KB, ROM < 45KB)
* console
* run js file

### Dependencies

* jsengine
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
# generate jsengine_app@developerkit default config
aos make jsengine_app@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make

# upload
aos upload
```

### Result

**jsengine_app** works with [Embeded JS WorkStation](https://linkdevelop.aliyun.com/deviceapp#/s):

![](https://img.alicdn.com/tfs/TB1hPA7pr2pK1RjSZFsXXaNlXXa-1920-938.jpg)

## Reference

* [jsengine Connect To LD](https://linkdevelop.aliyun.com/device-doc#ebagtb.html)
* [AliOS-Things JSEngine](../../../middleware/jsengine)
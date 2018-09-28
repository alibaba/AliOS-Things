## Developer Kit

### Content
- [Developer Kit](#developer-kit)
    - [Content](#content)
    - [Overview](#overview)
    - [Hardware Overview](#hardware-overview)
    - [AliOS Things Supported Features](#alios-things-supported-features)
    - [Download and Debug](#download-and-debug)
    - [MW31 Moudle Upgrade](#MW31 moudle upgrade)

### Overview

**Developer Kit** is hardware development board base on AliOS, with a variety of peripherals. Hardware is equipped with Audio Codec, eight sensors, 8-bits digital camera, LCD display, sixes LEDs, PCIe module, USB OTG FS, and Wi-Fi module. Because the software is embedded with the AliOS system, the local data uploading to the Ali-cloud can be realized through Wi-Fi. It is convenience to the developer to learn about the development environment in combination with practical applications.

![](https://img.alicdn.com/tfs/TB122RCtntYBeNjy1XdXXXXyVXa-2373-3121.png)

More information about the board can be found at here:

- [AliOS Things github wiki](https://github.com/alibaba/AliOS-Things/wiki)
- [AliOS Things Developer Kit Hardware Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-Hardware-Guide)
- [AliOS Things Developer Kit User Basic Operation Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-User-Basic-Operation-Guide)
- [AliOS Things Developer Kit Quick Start Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-Quick-Start-Guide)

### Hardware Overview

* cpu: STM32L496AGI6
    * arch: Cortex-M4 with FPU
    * frequency: 80 MHz
* D-Camera: 640*480 , 8-bits parallel interface
* flash: 1 MB
* SRAM: 320 KB
* Audio: Speech recognition resolution, Headphone and Microphone
* SDMMC: support 4bits mode, Maximum support for 32G
* LED: 1RGB LED ,5 user LEDs
* Four Keys: one reset key and three for user
* sensor
    * 3D Digital accelerometer
    * Gyroscope
    * Compass sensor
    * Pressure sensor
    * Humidity and Temperature Sensor
    * IR detector and emitter
    * Digital ALS and Proximity sensor
* Micro-USB: 5V power supply
* debug interface: serial wire debug (SWD), JTAG, Embedded Trace Macrocell*


### AliOS Things Supported Features

**flash partitions:**

|start Addr   |size             |read|write|descript|
|-------------|-----------------|:-:|:-:|-----------|
|0x08000000   |0x5C000 (368k)   |:o:|:o:|Application|
|0x0805C000   |0x1000 (4k)      |:o:|:o:|PARAMETER1 |
|0x0805D000   |0x2000 (8k)      |:o:|:o:|PARAMETER2 |
|0x08060000   |0x5C000 (368k)   |:o:|:o:|OTA Storage|
|0x080BC000   |0x1000 (4k)      |:o:|:o:|PARAMETER3 |
|0x080BD000   |0x1000 (4k)      |:o:|:o:|PARAMETER4 |
|0x080C0000   |0x40000 (256k)   |:o:|:o:|spiffs     |

**LCD:**

**buttons and LEDs:**

**sensor:**

**camera:**

**audio:**

### Download and Debug

check [AliOS Things Developer Kit Quick Start Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-Quick-Start-Guide#24-%E5%BC%80%E5%A7%8B%E7%83%A7%E5%86%99) for more information.

### MW31 Moudle Upgrade

Usage Guide Doc: [MW31 Upgrade Doc Guide](https://github.com/aliosthings/wiki/blob/master/external/MW31_Upgrade_Guide.md)

Video Guide: [MW31 Upgrade Video Guide](https://v.youku.com/v_show/id_XMzc1NjE4NzI2MA==.html?spm=a2hzp.8253869.0.0)

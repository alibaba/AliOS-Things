## Overview

**Developer Kit** is hardware development board base on [AliOS-Things](https://github.com/alibaba/AliOS-Things), with a variety of peripherals. Hardware is equipped with Audio Codec, eight sensors, 8-bits digital camera, LCD display, sixes LEDs, PCIe module, USB OTG FS, and Wi-Fi module. Because the software is embedded with the AliOS system, the local data uploading to the Ali-cloud can be realized through Wi-Fi. It is convenience to the developer to learn about the development environment in combination with practical applications.

More information about the board can be found at here:

- [AliOS Things github wiki](https://github.com/alibaba/AliOS-Things/wiki)
- [AliOS Things Developer Kit Hardware Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-Hardware-Guide)
- [AliOS Things Developer Kit User Basic Operation Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-User-Basic-Operation-Guide)
- [AliOS Things Developer Kit Quick Start Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-Quick-Start-Guide)


### Directories

```sh
developerkit
├── aos         # aos interface
├── camera_hal  # camera driver and demo
│   └── include
├── hal         # stm32l4 hal headers
├── Inc
├── irda_hal    # irda driver
│   └── include
├── mbmaster_hal# modbus master serial port
├── pwrmgmt_hal # low power manager
├── Src         # hal init
├── startup_stm32l496xx.s           # stm32l496xx startup file
├── STM32L496VGTx_FLASH_app.ld      # app link file for multi-bins
├── STM32L496VGTx_FLASH_kernel.ld   # kernel link file for multi-bins
└── STM32L496VGTx_FLASH_app.ld      # link file
```

### Board Hardware Resources

* CPU: STM32L496AGI6
    * arch: Cortex-M4 with FPU
    * frequency: 80 MHz
* D-Camera: 640*480 , 8-bits parallel interface
* Flash: 1 MB
* SRAM: 320 KB
* Audio: Speech recognition resolution, Headphone and Microphone
* SDMMC: support 4bits mode, Maximum support for 32G
* LED: 1 RGB LED ,5 user LEDs
* 4 Keys: 1 reset key and 3 for user
* Sensors
    * 3D Digital accelerometer
    * Gyroscope
    * Compass sensor
    * Pressure sensor
    * Humidity and Temperature Sensor
    * IR detector and emitter
    * Digital ALS and Proximity sensor
* Micro-USB: 5V power supply
* Debug Interface: serial wire debug (SWD), JTAG, Embedded Trace Macrocell

More information, please check: [AliOS Things Developer Kit Hardware Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-Hardware-Guide).


### Flash Partitions

|start Addr   |size             |descript|
|-------------|-----------------|-----------|
|0x08000000   |0x5C000 (368k)   |Application|
|0x0805C000   |0x1000 (4k)      |PARAMETER1 |
|0x0805D000   |0x2000 (8k)      |PARAMETER2 |
|0x08060000   |0x5C000 (368k)   |OTA Storage|
|0x080BC000   |0x1000 (4k)      |PARAMETER3 |
|0x080BD000   |0x1000 (4k)      |PARAMETER4 |
|0x080C0000   |0x40000 (256k)   |spiffs     |

### Flashing and debugging

check [AliOS Things Developer Kit Quick Start Guide](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Developer-Kit-Quick-Start-Guide#24-%E5%BC%80%E5%A7%8B%E7%83%A7%E5%86%99) for more information.

### Reference

Usage Guide Doc: [MW31 Upgrade Doc Guide](https://github.com/aliosthings/wiki/blob/master/external/MW31_Upgrade_Guide.md)

Video Guide: [MW31 Upgrade Video Guide](https://v.youku.com/v_show/id_XMzc1NjE4NzI2MA==.html?spm=a2hzp.8253869.0.0)
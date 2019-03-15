## developerkitaudio

### Content
- [developerkitaudio](#developerkitaudio)
    - [Content](#content)
    - [Overview](#overview)
        - [Dependent components](#dependent-components)
    - [Supported boards](#supported-boards)
    - [Build](#build)
    - [Run](#run)

### Overview
> 中文版见这里。Please see [here](https://github.com/Notioni/Codes/tree/master/Developer%20Kit/Codec%E5%9B%BA%E4%BB%B6) to find usage guide in chinese.  

`developerkitaudio` is a the specific example for developerkit, It demonstrate how to operate the codec called ISD9160. developerkitaudio source files:

```sh
developerkitaudio
    ├── developerkitaudio.c
    └── developerkitaudio.mk
    └── ucube.mk
```

#### Dependent components

`yloop`  `cli` `board.developerkit.audio_hal`

### Supported Boards

- developerkit

### Build

using [aos-cube](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uCube) to build developerkitaudio for [developerkit](../../board/developerkit/README.md)
More details for [Environment Setup](https://linkdevelop.aliyun.com/device-doc#dev-prepare.html)  

```sh
# generate dk.dk_audio@developerkit default config
aos make dk.dk_audio@developerkit -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

### Prepare and Flash

 You can get ISD9160 firmware which is for codec from [here](https://github.com/Notioni/Codes/tree/master/Developer%20Kit/Codec%E5%9B%BA%E4%BB%B6).   
 Two different firmware version as below:

`v1.xx` ---- record and playback via I2S.
`v2.xx` ---- voice recognition.

Please copy firmware for voice codec on a sdcard, rename it to `isd9160_fw.bin` one by one, and install the sdcard to developerkit.
E.g, if you want to record or playback voice, please use v1.xx.

Connect Developerkit ST-LINK USB port to PC to flash image to MCU.
```sh
aos upload developerkitaudio@developerkit
```

### Run

Press `key B` to upgrade firmware of codec(ISD9160) that is stored on SD card, there are some logs from terminal.

* If using v1.xx codec firmware, please follow below to record/playback voice:  
  Press `key M` to record to file that is stored on SD card.     
   
  Press `key A` to playback from file that is stored on SD card. 
 
* If using v2.xx codec firmware, please follow below to recognize the meaning of voice: 

Try to speak the wakeup command "Xiao Te Xiao Te", the LED1 will be turn on for about 10 seconds if waked up, then you can speak special commands at this moment that LED1 is on-line.
When the LED1 is off, you need to wake it up again.

[Recognition command list](https://github.com/Notioni/Codes/blob/master/Developer%20Kit/Codec%E5%9B%BA%E4%BB%B6/readme.txt), currently only support chinese.

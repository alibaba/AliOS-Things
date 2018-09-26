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

`developerkitaudio` is a the specific example for developerkit, It demonstrate how to operate the codec called ISD9160. developerkitaudio source files:

```sh
developerkitaudio
    ├── developerkitaudio.c
    └── developerkitaudio.mk
    └── ucube.mk
```

#### Dependent components

`yloop`  `cli` `board.developerkit.audio_hal`

### Supported boards

`developerkitaudio` only support the developerkit.

### Build

using [aos-cube](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-uCube) to build developerkitaudio for [developerkit](../../board/developerkit/README.md):

```sh
aos make developerkitaudio@developerkit
```

### Run

You can take the ISD9160 firmware from [http://www.notioni.com/#/source](http://www.notioni.com/#/source). The firmware comes 2 versions:

`v1.xx` ---- record and playback via I2S.
`v2.xx` ---- voice recognition.

Please copy the firmware on a sdcard, rename it to `isd9160_fw.bin`, and install the sdcard to developerkit.

Connect Developerkit ST-LINK USB port to PC.

```sh
aos upload developerkitaudio@developerkit
```

Then open the developerkit ST-LINK uart, and you can:

Press `key B` to upgrade firmware of codec(ISD9160) that is stored on SD card.

Press `key M` to record to file that is stored on SD card.                                     *---- for v1.xx*

Press `key A` to playback from file that is stored on SD card.                                 *---- for v1.xx*

Try speak the command "Xiao Te Xiao Te" to awake, and the ST-LINK uart will echo the command, while the LED1 will be lit on for 10 seconds, then you can speak other commands during the time.
When the LED1 is off, you need to awake it again.                                              *---- for v2.xx*

Recognition command list and more details are available in [http://www.notioni.com/#/source](http://www.notioni.com/#/source).
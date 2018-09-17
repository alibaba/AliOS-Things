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

Connect Developerkit ST-LINK USB port to PC.

```sh
aos upload developerkitaudio@developerkit
```

Then open the developerkit ST-LINK uart, and you can:

Press `key B` to upgrade firmware of codec(ISD9160) that is stored on SD card.

Press `key M` to record to file that is stored on SD card.

Press `key A` to playback from file that is stored on SD card.
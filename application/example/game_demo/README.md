## Contents

```sh
game_demo
├── appdemo.c
├── Config.in       # kconfig file
├── aos.mk          # aos build system file(for make)
├── sdl2-tetris     # tetris game
└── ucube.py        # aos build system file(for scons)
```

## Introduction

The **game_demo** example will print `hello world` and other information every 5 seconds, the example is simple.

### Dependencies


* osal_aos
* SDL2
* freetype253
* littlefs
* cli
* netmgr
* lcd

### Supported Boards

- all

### Build

```sh
# generate game_demo@developerkit default config
aos make game_demo@haas100 -c config

# build
cp application/example/game_demo/sdl2-tetris/data/DejaVuSerif.ttf ./platform/mcu/haas1000/prebuild/data/
aos make
```

## Reference

* [Quick-Start](https://github.com/alibaba/AliOS-Things/wiki/Quick-Start)

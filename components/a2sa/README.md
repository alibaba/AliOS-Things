@page a2sa a2sa

# 概述
A2SA是Advanced AliOS Things Sound Architucture的缩写，是AliOS Things音频系统服务框架，包括音频驱动层、音频服务接口层以及音频插件管理等。部分设计理念借鉴了Linux ALSA的设计思路，例如在设备驱动层支持VFS设备驱动挂载访问、支持多声卡管理、PCM/Control音频设备模型、对音频Codec友好抽象等。在音频服务接口层兼容Linux ALSA-Lib的API接口规范,方便基于ALSA-Lib开发的应用组件移植到AliOS Things。同时A2SA的footprint也足够小（23KB），适合在嵌入式IoT设备上移植。
组件支持以下功能：
- 最大程度兼容Linux ALSA-Lib API规范，方便Linux应用生态移植到AliOS Things。
- 支持多声卡模式（蓝牙声卡，I2S声卡等）
- 驱动挂载VFS，支持open/ read/ write等操作接口。
- 精简的Audio Codec驱动接口抽象（共11个API），可以方便的对接不同Codec HAL驱动。
- 兼容Linux ALSA音频驱动的适配正在开发中。

## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── src
│   ├── driver               # 音频驱动框架
│   │   ├── core             # 音频驱动框架Core
│   │   └── platform         # 不同平台音频驱动硬件抽象层
│   │       ├── Linux        # Linux平台音频驱动硬件抽象层，开发中，后续发布。
│   │       └── RTOS         # RTOS平台音频驱动硬件抽象层，共11个API抽象，支持低功耗管理。
│   └── framework            # 音频服务接口API
│       ├── sound_mixer.c    # 声卡mixer设备驱动的实现
│       └── sound_pcm.c      # 声卡pcm设备驱动的实现
├── include
│   ├── sound_mixer.h        # 声卡mixer设备驱动，例如设置codec参数，音量等。
│   └── sound_pcm.h          # 声卡pcm设备驱动，例如录音、播放等。
├── internal                 # 内部头文件
├── package.yaml             # 编译配置文件
└── example
    ├── sound_example.c      # a2sa示例代码
    ├── sound_example_lb.c   # loopback测试代码
    ├── sound_example_vol.c  # volume测试代码
    └── sound_example_wav.c  # wav播放器测试代码
```

## 依赖组件

* rhino
* vfs
* ulog
* posix

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> 最多支持的声卡个数: 默认5, 如需修改，在yaml中修改AOS_SNDCARD_NUM_MAX配置
```sh
def_config:
  AOS_SNDCARD_NUM_MAX: 5
```
> 每个声卡最多支持的音频设备节点数，默认5，可按照实际需要修改yaml配置如：
```sh
def_config:
  AOS_SNDCARD_DEVICE_NUM_MAX: 5
```

# API说明
@ref a2sa_pcm_api
@ref a2sa_mixer_api

# 使用示例
示例代码参考example/sound_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> a2sa组件的package.yaml中添加example示例代码
```sh
source_file:
  - "src/*.c"
  - example/sound_example.c # add sound_example.c
  - example/sound_example_lb.c
  - example/sound_example_vol.c
  - example/sound_example_wav.c
```

## app中添加a2sa组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - a2sa: master # helloworld_demo中引入a2sa组件
def_config:
  CONFIG_A7_DSP_ENABLE: 1 # 使能HaaS100音频驱动开关
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档。

## A2SA示例测试

### CLI命令行输入：
```sh
sound_install_driver # 安装默认声卡
```

> CLI关键日志：
```sh
sound install driver test begin ...
sound install driver test end !!!
```

### CLI命令行输入：
```sh
sound_loopback start # 开始声卡录音播放loopback测试
```

> CLI关键日志：
```sh
sound loopback test begin ...
```

### CLI命令行输入：
```sh
sound_loopback stop # 停止声卡录音播放loopback测试
```

> CLI关键日志：
```sh
sound loopback test end !!!
```

### CLI命令行输入：
```sh
sound_setvol 80 # 设置播放音量为80
```

> CLI关键日志：
```sh
alsa_volume_set percent 80
```

### CLI命令行输入：
```sh
sound_getvol # 设置播放音量为80
```

> CLI关键日志：
```sh
get volume 80
```

### CLI命令行输入：
```sh
sound_wav /data/7.wav # 播放/data/7.wav的音频，仅限于.wav格式的音频
```

> CLI关键日志：
```sh
open /data/7.wav successfully
```

# FAQ

Q1： 必须通过sound_install_driver手动安装声卡吗？

答：由于不同的硬件环境，声卡类型不同，声卡支持的Capture & Playback的通道数都不同，开发者可以根据实际的板子硬件在代码合适的位置通过调用audio_native_card_register()函数来安装声卡，建议参考audio_install_codec_driver()的实现。


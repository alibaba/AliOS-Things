@page a2sa a2sa

[更正文档](https://gitee.com/alios-things/a2sa/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

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
```tree
.
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
系统中相关配置已有默认值，如需修改配置，统一在该组件的package.yaml中 **def_config** 节点修改，具体如下：
> 最多支持的声卡个数: 默认5, 如需修改，在package.yaml中修改AOS_SNDCARD_NUM_MAX配置

```yaml
def_config:
  AOS_SNDCARD_NUM_MAX: 5
```
> 每个声卡最多支持的音频设备节点数，默认5，可按照实际需要修改package.yaml配置如：

```yaml
def_config:
  AOS_SNDCARD_DEVICE_NUM_MAX: 5
```

# API说明
- 参考 [a2sa_pcm_api](https://g.alicdn.com/alios-things-3.3/doc/group__a2sa__pcm__api.html)
- 参考 [a2sa_mixer_api](https://g.alicdn.com/alios-things-3.3/doc/group__a2sa__mixer__api.html)

# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml

depends:
  - a2sa: master          # helloworld_demo中引入a2sa组件
  - littlefs: master      # a2sa组件wav播放器示例依赖littlefs文件系统组件

def_config:
    CONFIG_A7_DSP_ENABLE: 1    # 必须：使能HaaS100音频驱动开关
    CLI_IOBOX_ENABLE: 1        # 可选：使能文件系统测试命令，例如ls, ll, mkdir, touch, echo等

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install a2sa

```

上述命令执行成功后，组件源码则被下载到了./components/a2sa路径中。

## 步骤4 添加示例

在a2sa组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/a2sa/tree/master/example)：

```yaml
source_file:
  - "src/*.c"
  - example/sound_example.c
  - example/sound_example_lb.c
  - example/sound_example_vol.c
  - example/sound_example_wav.c
```

其中sound_example_wav.c是播放本地文件系统中/data/7.wav音频文件的示例。如果期望演示该播放wav文件的示例，则需要按下面2个步骤打包.wav音频文件到文件系统中。

```shell
cp ./6.wav ./hardware/chip/haas1000/prebuild/data/  # 拷贝当前目录下的6.wav文件到文件系统打包目录./hardware/chip/haas1000/prebuild/data/下。注意：6.wav文件需要自己准备。
```

```yaml
# 确认./hardware/chip/haas1000/package.yaml文件中第177行至179行是打开的状态。如下，
program_data_files:
   - filename: release/write_flash_tool/ota_bin/littlefs.bin
     address: 0xB32000
```


## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

**CLI命令行输入：**
```shell

sound_install_driver # 安装默认声卡

```

> 关键日志：
```shell

sound install driver test begin ...
sound install driver test end !!!

```

**CLI命令行输入：**
```shell

sound_loopback start # 开始声卡录音播放loopback测试

```

> 关键日志：
```shell

sound loopback test begin ...

```

**CLI命令行输入：**
```shell

sound_loopback stop # 停止声卡录音播放loopback测试

```

> 关键日志：
```shell

sound loopback test end !!!

```

**CLI命令行输入：**
```shell

sound_setvol 80 # 设置播放音量为80

```

> CLI关键日志：
```shell

alsa_volume_set percent 80

```

**CLI命令行输入：**
```shell

sound_getvol # 设置播放音量为80

```

> 关键日志：
```shell

get volume 80

```

**CLI命令行输入：**
```shell

sound_wav /data/7.wav 1 # 播放/data/7.wav的音频1次（可修改），仅限于.wav格式的音频

```

> 关键日志：
```shell

open /data/7.wav successfully

```

# FAQ

Q1： 必须通过sound_install_driver手动安装声卡吗？

答：由于不同的硬件环境，声卡类型不同，声卡支持的Capture & Playback的通道数都不同，开发者可以根据实际的板子硬件在代码合适的位置通过调用audio_native_card_register()函数来安装声卡，建议参考audio_install_codec_driver()的实现。


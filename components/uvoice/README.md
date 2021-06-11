@page uvoice uvoice

[更正文档](https://gitee.com/alios-things/uvoice/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
uVoice是AliOS Things的音频组件，提供了RTOS系统上端到云全链路的语音能力，包括音频硬件管理、音频增强算法、播放器、录音等基本音频功能，也结合阿里的智能语音云端服务，提供了TTS、语音控制、语音对话等能智能语音功能。同时，uVoice也是一个资源占用极少，性能较高的音频组件，适合为低资源、低算力的嵌入式MCU提供音频能力。
组件支持以下功能：
- 录音
- 播放(Flash/Memory/FS/HTTP/HTTPS/HLS)
- 播放控制(暂停/继续/切换/快进/快退/播放列表)
- 音频格式解码(MP3、AAC等)
- 音频格式编码
- 阿里云TTS

## 版权信息
> Apache license v2.0

## 目录结构
```tree
.
├── aos.mk
├── application
│   └── alicloudtts
│       ├── alicloudtts.c
│       ├── alicloudtts.h
│       ├── alicloudtts_intf.h
│       ├── include
│       │   └── uvoice_tts.h
│       └── tts.c
├── audio
│   ├── audio_aec.c
│   ├── audio_aec.h
│   ├── audio_common.c
│   ├── audio_common.h
│   ├── audio_mixer.c
│   ├── audio_mixer.h
│   ├── audio_process.c
│   ├── audio_process.h
│   ├── audio_stream.c
│   ├── audio_stream.h
│   ├── audio_trigger.c
│   ├── audio_trigger.h
│   ├── audio_vad.c
│   ├── audio_vad.h
│   ├── hal
│   │   ├── haas100
│   │   │   └── uvoice_pcm.c  # haas100 
│   │   └── linux
│   │       └── uvoice_pcm.c
│   └── process
│       ├── proc_aec.c
│       ├── proc_agc.c
│       ├── proc_ns.c
│       ├── proc_resample.c
│       └── proc_vad.c
├── build
│   └── mk_makefile
├── CMakeLists.txt
├── codec
│   ├── amr_decoder.c
│   ├── amr_encoder.c
│   ├── amrwb_decoder.c
│   ├── amrwb_encoder.c
│   ├── ogg_decoder.c
│   ├── opensource
│   │   ├── pvaac                    # aac解码器
│   │   │   ├── aos.mk
│   │   │   ├── get_pv_aac.py
│   │   │   ├── oscl
│   │   │   │   ├── oscl_base.h
│   │   │   │   ├── oscl_dll.h
│   │   │   │   ├── oscl_exception.h
│   │   │   │   └── oscl_mem.h
│   │   └── pvmp3                     # mp3解码器
│   │       ├── aos.mk
│   │       ├── CMakeLists.txt
│   │       ├── get_pvmp3.py
│   │       ├── Makefile
│   │       ├── oscl_base.h
│   │       └── oscl_mem.h
│   ├── opus_decoder.c
│   ├── opus_encoder.c
│   ├── pvaac_decoder.c
│   ├── pvmp3_decoder.c
│   ├── spx_decoder.c
│   └── spx_encoder.c
├── common
│   ├── base64.c
│   ├── event.c
│   ├── message.c
│   ├── ringbuffer.c
│   └── urlencode.c
├── Config.in
├── connect
│   ├── uvoice_ws.c
│   └── uvoice_ws.h
├── example
│   └── uvoice_example.c             # 示例代码
├── include                          # 对外头文件
│   ├── uvoice_event.h
│   ├── uvoice_init.h
│   ├── uvoice_mlist.h
│   ├── uvoice_player.h
│   ├── uvoice_recorder.h
│   ├── uvoice_test.h
│   └── uvoice_types.h
├── internal                         # 内部头文件
│   ├── uvoice_alios.h
│   ├── uvoice_amp.h
│   ├── uvoice_audio.h
│   ├── uvoice_codec.h
│   ├── uvoice_common.h
│   ├── uvoice_config.h
│   ├── uvoice_format.h
│   ├── uvoice_linux.h
│   ├── uvoice_list.h
│   ├── uvoice_message.h
│   ├── uvoice_os.h
│   ├── uvoice_pcm.h
│   ├── uvoice_play.h
│   ├── uvoice_record.h
│   ├── uvoice_resampler.h
│   ├── uvoice_ringbuffer.h
│   ├── uvoice_stream.h
│   └── uvoice_wave.h
├── media
│   ├── uvoice_codec.c
│   ├── uvoice_fade.c
│   ├── uvoice_format.c
│   ├── uvoice_mlist.c
│   ├── uvoice_player.c
│   ├── uvoice_recorder.c
│   ├── uvoice_stream.c
│   └── uvoice_wave.c
├── package.yaml
├── README.md
├── stream
│   ├── uvoice_cache.c
│   ├── uvoice_cache.h
│   ├── uvoice_download.c
│   ├── uvoice_download.h
│   ├── uvoice_file.c
│   ├── uvoice_hls.c
│   ├── uvoice_hls.h
│   ├── uvoice_http.c
│   ├── uvoice_http.h
│   ├── uvoice_partition.c
│   └── uvoice_ssl.c
├── test
│   ├── test_main.c
│   ├── test_player.c
│   ├── test_recorder.c
│   ├── test_swid.c
│   └── test_tts.c
├── tools
│   └── gen_voice.py
└── uvoice.c
```

## 依赖组件
* a2sa
* ulog
* mbedtls

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> 支持播放功能开关
```yaml
  UVOICE_PLAYER_ENABLE: 1
```
> 支持录音功能开关
```yaml
UVOICE_RECORDER_ENABLE: 1
```
> 支持播放列表功能开关
```yaml
UVOICE_MLIST_ENABLE: 1
```
> 支持播放文件功能开关
```yaml
UVOICE_FILE_ENABLE: 1
```
> 支持播放HTTP功能开关
```yaml
UVOICE_HTTP_ENABLE: 1
```
> 支持播放HTTPS功能开关
```yaml
UVOICE_HTTPS_ENABLE: 1
```
> 支持播放HTTPS功能开关
```yaml
UVOICE_HTTPS_ENABLE: 1
```
> 支持播放网络音频时自动下载功能开关
```yaml
UVOICE_DOWNLOAD_ENABLE: 1
```
> 支持播放HLS流媒体功能开关
```yaml
UVOICE_HLS_ENABLE: 1
```
> 支持MP3解码功能开关
```yaml
DECODER_PV_MP3_ENABLE: 1
```
> 支持AAC(M4A)解码功能开关
```yaml
DECODER_PV_M4A_ENABLE: 1
```
> 支持阿里云TTS功能开关
```yaml
ALICLOUD_TTS_SUPPORT: 1
```


# API说明

- 参考 [uvoice_init_api](https://g.alicdn.com/alios-things-3.3/doc/group__uvoice__init__api.html)
- 参考 [uvoice_types_api](https://g.alicdn.com/alios-things-3.3/doc/group__uvoice__types__api.html)
- 参考 [uvoice_player_api](https://g.alicdn.com/alios-things-3.3/doc/group__uvoice__player__api.html)
- 参考 [uvoice_recorder_api](https://g.alicdn.com/alios-things-3.3/doc/group__uvoice__recoder__api.html)
- 参考 [uvoice_mlist_api](https://g.alicdn.com/alios-things-3.3/doc/group__uvoice__mlist__api.html)
- 参考 [uvoice_event_api](https://g.alicdn.com/alios-things-3.3/doc/group__uvoice__event__api.html)
- 参考 [uvoice_test_api](https://g.alicdn.com/alios-things-3.3/doc/group__uvoice__test__api.html)

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

> solution中helloworld_demo的package.yaml中添加
```yaml
depends:
  - uvoice: master # helloworld_demo中引入uvoice组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install uvoice

```

上述命令执行成功后，组件源码则被下载到了./components/uvoice路径中。

## 步骤4 添加示例

> uvoice组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/uvoice/tree/master/example)：

```yaml
source_file:
  - "example/uvoice_example.c" # add uvoice_example.c
```

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

> uvoice示例代码初始化
```sh
uvoice_example
```
> uvoice播放文件系统MP3文件功能测试
```sh
play "fs:/data/test.mp3"      # 这里文件名替换成实际放在littlefs文件系统中的MP3文件
```
> uvoice播放文件系统m4a文件功能测试
```sh
play "fs:/data/test.m4a"      # 这里文件名替换成实际放在littlefs文件系统中的M4A文件
```
> uvoice播放网络音频功能测试
```sh
play "http:xxxxxxxx/test.mp3"      # 这里文件名替换成实际http链接
```
```sh
play "https:xxxxxxxxs/test.mp3"      # 这里文件名替换成实际https链接
```
> uvoice录音功能测试
```sh
record 16000 1 16 1024 0 fs:/data/recording.wav
```

**关键日志**
> CLI日志：
```sh
uvoice example initialization succeeded !
```

# 注意事项
> 解码器下载
```sh
要支持MP3播放，编译前，需要到codec/opensource/pvmp3目录下执行get_pvmp3.py
要支持AAC/M4A播放，编译前，需要到codec/opensource/pvaac目录下执行get_pvaac.py
```
> 支持播放列表功能
```sh
要支持播放列表功能，需要有SD卡，并使能fatfs文件系统
```
> 支持https
```sh
要支持https播放功能，需要将mbedtls的MBEDTLS_CONFIG_TLS_MAX_CONTENT_LEN配置为16k
```

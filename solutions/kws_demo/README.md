@page kws_demo HaaS AI之HaaS语音唤醒

[更正文档](https://gitee.com/alios-things/kws_demo/edit/rel_3.3.0/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://help.aliyun.com/document_detail/302301.html)

# 1. 案例简介
kws(keyword spooting)是基于本地离线唤醒引擎，主要有三个部分组成：
1. 接入模拟麦克风(Mic3输入)，采集HaaS HaaS唤醒词；
2. 当唤醒后，首先HaaS100上进行LED跑马灯闪烁；
3. 接入喇叭，播放本地响应词(mp3)。


# 2. 基础知识
## 2.1 基础目录结构
```tree
├── cp_resources.py     # 拷贝本地语料到/prebuild/data目录，编译进文件系统
├── kws.c               # 唤醒词(keyword spooting)响应处理逻辑代码
├── kws.h               # 唤醒词(keyword spooting)头文件
├── main.c              # 该solution核心打印输出代码，入口**application_start**
├── maintask.c          # 系统主任务入口处理，入口**aos_maintask**
├── Makefile            # aos make编译时入口
├── package.yaml        # 编译系统配置文件
├── README.md           # solution使用指南
├── resources           # 本地mp3语料
│   └── mp3
└── SConstruct          # Makefile => Scon => aostools
```

# 3. 方案介绍
## 3.1 硬件搭建
**接线图**
<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01T0U0jL28B2pYA09qL_!!6000000007893-2-tps-972-674.png"  style="max-width:600px;" />
</div>

**HaaS100硬件排线图请参考**
https://help.aliyun.com/document_detail/184186.html?spm=a2c4g.11186623.6.703.274c51344iOgNc

购买链接仅供参考！！我们不负责商家发货的品质保障等问题！！

| 名称        |   数量   |  参考链接  |
| ------  | :-----:  | :----:  |
| HaaS100开发版 | 1 |   [HaaS100购买链接](https://detail.tmall.com/item.htm?spm=a220m.1000858.1000725.2.38a23a98TnoFxv&id=627354125244&user_id=4160407286&cat_id=2&is_b=1&rn=bb0ed71a917987289b7d66f1803f4efc)     |
| microUSB数据线        |   1   |   普通microusb线即可   |
| 模拟MIC        |    1    |  [模拟MIC参考链接](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.795a2e8drZ42nl&id=583316469629&_u=531h6c618fe)  |
| 喇叭        |   1   |   [喇叭参考链接](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.6c4e2e8de1Ubch&id=564311744050&_u=ob3iuit4288)   |


## 3.2 软件架构
<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN016gzTt31mgGHLJEzSi_!!6000000004983-2-tps-664-446.png"  style="max-width:600px;" />
</div>

> * kws_demo: 主要打通实现AI语音引擎的初始化，以及处理唤醒后的跑马灯效果和语音响应播报。
> * ai_agent组件：是AliOS Things上的AI引擎核心模块，后端接入不同的推理引擎，本案例中在初始化ai_agent时选择AI_MODEL_KWS(keyword spooting)。
> * uVoice组件：是AliOS Things上智能语音解决方案的核心组件，提供了本地音频，URL音频，TTS合成等基础功能，音频格式支持mp3, m4a, wav, opus等主流格式，本案例中使用它来进行本地mp3语料的响应播报。
> * A2SA组件：是AliOS Things上音频服务框架，兼容ALSA应用接口访问，支持音频硬件驱动抽象，多音频驱动加载/卸载，VFS接口支持等功能。

# 4. AliOS Things开发环境搭建
案例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具，所以首先需要参考[《AliOS Things集成开发环境使用说明之搭建开发环境》](https://help.aliyun.com/document_detail/302378.html)，下载安装。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 4.7 案例代码下载
该案例相关的源代码下载可参考[《AliOS Things集成开发环境使用说明之创建工程》](https://help.aliyun.com/document_detail/302379.html)。
其中：
> 选择解决方案: “HaaS语音唤醒案例”或者“kws_demo”

> 选择开发板: HaaS100

## 4.8 代码编译、烧录
-- 固件编译方法可参考[《AliOS Things集成开发环境使用说明之编译固件》](https://help.aliyun.com/document_detail/302384.html)。

### 4.8.1 文件件系统烧录
本组件例子中使用到到的本地语料存放在代码中hardware/chip/haas1000/prebuild/data/目录下mp3目录，除烧录kws_demo image外，需烧录littlefs文件系统，请将hardware/chip/haas1000/package.yaml文件中以下代码段的注释打开：

```yaml
  program_data_files:
    - filename: release/write_flash_tool/ota_bin/littlefs.bin
      address: 0xB32000
```

-- 固件烧录方法可参考[《AliOS Things集成开发环境使用说明之烧录固件》](https://help.aliyun.com/document_detail/302383.html)。

### 4.9 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《AliOS Things集成开发环境使用说明之查看日志》](https://help.aliyun.com/document_detail/302382.html)。

# 5. 案例演示
<video width="100%" height="560"  controls="controls" src="https://cloud.video.taobao.com//play/u/3903519387/p/2/e/6/t/1/312916213915.mp4"></video>

当程序烧录完成后，直接喊出“HaaS HaaS"，就可以看到视频所示的效果。目前只支持近场唤醒，唤醒距离1米左右。如果听不到声音请逆时针选择喇叭蓝色旋钮调节音量至最大。

# 6. 总结
本文提供了本地语音唤醒体验的案例，用户可以根据需要在demo中修改唤醒后的逻辑处理，比如唤醒后直接播放音乐或者控制GPIO等操作，后续我们将有更多本地唤醒词输出，快来体验一下吧。

@page tflite_micro_speech_demo TFLite-Micro语音快捷词唤醒案例

[更正文档](https://gitee.com/alios-things/tflite_micro_speech_demo/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 1. 案例简介
本案例是在HaaS EDU K1上部署基于TFLite-Micro本地语音识别推理模型，主要有三个部分组成：
1. 语音采集：接入模拟麦克风(Mic1输入)；
2. 语音识别：说出“打开”和“关闭”识别后，OLED将显示“Turn on...”和“Turn off”；
3. 语音播报：执行指令的同时，播报本地TTS(mp3)。

# 2. 基础知识
## 2.1 基础目录结构
```tree
├── cp_resources.py     # 拷贝本地语料到/prebuild/data目录，编译进文件系统
├── main.c
├── maintask.c
├── Makefile
├── micro_speech        # 语音识别程序
├── oled                # OLED显示程序
│   ├── oled.c
│   └── oled.h
├── package.yaml        # 编译系统配置文件
├── player              # 播放器程序
│   ├── player.c
│   └── player.h
├── README.md
├── recorder            # 录音程序
│   ├── recorder.c
│   └── recorder.h
├── resources
│   └── mp3             # 本地mp3语料
├── SConstruct
```

# 3. 方案介绍
## 3.1 硬件搭建

如果有HaaS语音扩展板，直接插入即可：
<div align=center>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01Lflv6j1bLgjU82zb0_!!6000000003449-0-tps-4032-3024.jpg" style="zoom:50%;" />
</div>

如果没有HaaS语音扩展板，请按照如下步骤接入麦克风和喇叭：

**接线图**
<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01XKHM5s1djcdqTJo1S_!!6000000003772-2-tps-1394-724.png" style="zoom:50%;" />
</div>

**HaaS EDU K1硬件排线图请参考**
https://help.aliyun.com/document_detail/205267.html

购买链接仅供参考！！我们不负责商家发货的品质保障等问题！！

| 名称        |   数量   |  参考链接  |
| ------  | :-----:  | :----:  |
| HaaS EDU K1开发版 | 1 |   [HaaS EDU K1购买链接](https://detail.tmall.com/item.htm?spm=a230r.1.14.16.71c3cf45V4cFdQ&id=639294516891&ns=1&abbucket=10)     |
| microUSB数据线        |   1   |   普通microusb线即可   |
| 模拟MIC        |    1    |  [模拟MIC参考链接](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.795a2e8drZ42nl&id=583316469629&_u=531h6c618fe)  |
| 喇叭        |   1   |   [喇叭参考链接](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.6c4e2e8de1Ubch&id=564311744050&_u=ob3iuit4288)   |


## 3.2 软件架构
<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01a7H3j41CtFMJHGUnP_!!6000000000138-2-tps-542-362.png"  style="max-width:600px;" />
</div>

> * KWS Demo应用程序: 主要打通实现AI语音引擎的初始化，欢迎语播报。
> * ai_agent组件：是AliOS Things上的AI引擎核心模块，后端接入不同的推理引擎，本案例中使用了TFLite-Micro推理引擎。
> * uVoice组件：是AliOS Things上智能语音解决方案的核心组件，提供了本地音频，URL音频，TTS合成等基础功能，音频格式支持mp3, m4a, wav, opus等主流格式，本案例中使用它来进行本地mp3语料的响应播报。
> * A2SA组件：是AliOS Things上音频服务框架，兼容ALSA应用接口访问，支持音频硬件驱动抽象，多音频驱动加载/卸载，VFS接口支持等功能。

# 4. AliOS Things开发环境搭建
案例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **haas-studio** ，所以首先需要参考[《HaaS开发环境》](https://g.alicdn.com/alios-things-3.3/doc/haas-studio.html?spm=a2cti.24227744.0.0.202e7468zZfbG0)，下载安装 **haas-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 4.7 案例代码下载
该案例相关的源代码下载可参考[《创建工程》](https://g.alicdn.com/alios-things-3.3/doc/haas-studio.html?spm=a2cti.24227744.0.0.696b40faTNQpKM#autotoc_md1867)。
其中：
> 选择解决方案: “TFLite-Micro离线语音快捷词唤醒案例”或者“tflite_micro_speech_demo”

> 选择开发板: HaaS EDU K1

## 4.8 代码编译、烧录
-- 固件编译方法可参考[《开发及调试》](https://g.alicdn.com/alios-things-3.3/doc/haas-studio.html?spm=a2cti.24227744.0.0.696b40faTNQpKM#autotoc_md1871)。

### 4.8.1 文件件系统烧录
本组件例子中使用到到的本地语料存放在代码中hardware/chip/haas1000/prebuild/data/目录下mp3目录，除烧录tflite_micro_speech_demo image外，需烧录littlefs文件系统，请将hardware/chip/haas1000/package.yaml文件中以下代码段的注释打开：

```yaml
  program_data_files:
    - filename: release/write_flash_tool/ota_bin/littlefs.bin
      address: 0xB32000
```

-- 固件烧录方法可参考[《开发及调试》](https://g.alicdn.com/alios-things-3.3/doc/haas-studio.html?spm=a2cti.24227744.0.0.696b40faTNQpKM#autotoc_md1871)。

### 4.9 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《开发及调试》](https://g.alicdn.com/alios-things-3.3/doc/haas-studio.html?spm=a2cti.24227744.0.0.696b40faTNQpKM#autotoc_md1871)。

# 5. 案例演示
<video width="100%" height="560"  controls="controls" src="https://cloud.video.taobao.com/play/u/3903519387/p/1/e/6/t/1/324208098397.mp4"></video>

当程序烧录完成后，直接喊出“打开"，“关闭”，就可以看到视频所示的效果。目前只支持近场唤醒，唤醒距离1米左右。如果听不到声音请逆时针选择喇叭蓝色旋钮调节音量至最大。

# 6. 自训练唤醒词

## 6.1 语料采集
### 唤醒词录制
录制时看到“开始录音，请说话......”即可立即说出唤醒词，比如“打开”、“关闭”。由于我们检测一秒的唤醒词，所以在注意要在一秒内说完整整个唤醒词，录制一次后会自动回放确认是否录制完整，如果录制完整，按回车键继续下一次录制，如果录制不完整或有其他杂音，按其他任意键删除刚才的录音再继续下一次录制。

执行命令：
```yaml
python micro_speech/train/record.py
```
<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01KKk34X1qLQkHobW8L_!!6000000005479-2-tps-830-336.png" style="zoom:50%;" />
</div>

毫无疑问，这个教学案例是教你如何录制一个人的声音，如果想要达到商业化的识别率，就至少需要500人以上的声音录制。如果仅仅录制你一个人的唤醒词，那么仅识别你的声音是可以的，但其他人在唤醒时的成功率就会低很多。这个案例重点是教你了解唤醒词训练部署的原理。

### 背景噪音录制
为了更好的识别，需要录制一些背景噪音，模型训练时会学习唤醒词和背景噪音的差别。背景噪音可以录制1～2分钟。模型训练时会自动从中随机选择片段作为背噪加入唤醒词中进行学习。

执行命令：
```yaml
python micro_speech/train/record_noise.py
```
<div align=center>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01KKk34X1qLQkHobW8L_!!6000000005479-2-tps-830-336.png" style="zoom:50%;" />
</div>
录制背景噪音，放到dataset/_background_noise_目录。

## 6.2 模型训练
### PC端训练 
PC上在VSCode中使用notebook插件打开tflite_micro_speech_demo/micro_speech/train/train_micro_speech_model.ipynb进行其他唤醒词的训练。

>前提：
参考[《HaaS AI之VSCode中搭建Python虚拟环境》](https://blog.csdn.net/HaaSTech/article/details/113512377?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522162994555516780261935882%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=162994555516780261935882&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-2-113512377.ecpm_v1_rank_v29&utm_term=haas+tensorflow&spm=1018.2226.3001.4187)搭建环境。
参考[《在VSCode中搭建TensorFlow 2.0简单神经网络》](https://blog.csdn.net/HaaSTech/article/details/113546062?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522162994555516780261935882%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=162994555516780261935882&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v29_ecpm-1-113546062.ecpm_v1_rank_v29&utm_term=haas+tensorflow&spm=1018.2226.3001.4187)
安装Tensorflow 1.x版本，非2.0版本。

### 6.3 阿里云PAI平台训练
如果PC性能有限，使用阿里云PAI平台进行训练也是一个不错的选择，PAI-DSW是一款云端机器学习开发IDE，为您提供交互式编程环境，适用于不同水平的开发者。你可以根据根据需要选择个人版、GPU特价版或[探索者版（免费）](https://dsw-dev.data.aliyun.com/#/)，相关使用手册[DSW新手使用手册](https://tianchi.aliyun.com/forum/postDetail?spm=TODO.TODO.header.2.21864825A5pdfB&postId=121050)。
使用DSW个人版：
1.	登录[PAI控制台](https://pai.data.aliyun.com/console)。
2.	在左侧导航栏，选择模型开发和训练 > 交互式建模（DSW）。
3.	在页面左上方，选择目标地域。
4.	在Notebook建模服务页面，单击创建实例。
5.	在配置实例向导页面，配置参数，镜像选择tensorflow1.15-gpu-py36-cu101-ubuntu18.04版本。

## 6.4 模型部署
模型部署在HaaS EDU K1上，主要有三个步骤：
1.	模型替换：将模型文件model.cc替换micro_speech/micro_features/model.cc文件。
2.	标签更新：在cc替换micro_speech/micro_features/micro_model_settings.cc中，将标签名更换为你训练的快捷词，比如“打开”、“关闭”。由于标签与模型的输出张量元素是按照顺序进行匹配的，因此，需要按照将标签提供给训练脚本的顺序列出这些标签。
3.	业务逻辑更新：在micro_speech/command_responder.cc中根据标签更新相应的业务逻辑。目前在听到“打开”后，会打开HaaS EDU K1上R/G/B LED灯。你也可以修改逻辑比如通过WiFi打开远程的风扇或灯。这里可以充分发挥你的想象力打造一些比较有意思的场景应用。

# 7. 总结
本案例在HaaS EDU K1上基于TFLite-Micro推理引擎进行语音唤醒词的部署。也提供了从唤醒词采集到模型训练，模型部署的全链路开发流程，帮助您深入理解在低功耗MCU上如何进行离线语音识别的开发部署，期待您打造更多属于你的唤醒词应用。

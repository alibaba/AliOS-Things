@page audio_demo audio_demo

本文主要介绍如何基于HaaS100硬件平台搭建“云端钉一体”（阿里云IoT平台 + HaaS100 + 钉钉小程序）的智能语音播放器。该智能语音播放器包含如下功能，

- 小程序：加载/卸载HaaS100上的声卡模块。
- 小程序：TTS (Text to Speech)，智能语音合成功能。
- 小程序：开始/停止录音，录音文件路径/data/rec.pcm
- 小程序：音乐播放功能，例如音量调节/播放/暂停/上一首/下一首/播放列表等。
- 本地CLI: 音量调节，本地音乐/ 网络音乐播放（.mp3, .m4a等格式）等。
- 本地CLI: TTS (Text to Speech)，智能语音合成功能。


<br />演示视频链接：待补充。<br />

<a name="gjSAP"></a>
# 1、端测：硬件平台搭建
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/100557/1607582005158-29af6c25-a59d-4150-b774-aaaf521d2e3e.png#align=left&display=inline&height=448&margin=%5Bobject%20Object%5D&name=image.png&originHeight=1468&originWidth=2130&size=1272796&status=done&style=none&width=650)<br />图1: HaaS100智能语音播放器系统框架<br />
<br />基于HaaS100开发“云端钉一体”的智能语音播放器，需要的硬件模块包括：

- **HaaS100开发版**（电源/ WIFI天线）: 购买HaaS100硬件套装时已经提供。
- **microUSB数据线**。
- **模拟MIC**：硅麦一般有正负两级，HaaS100和硅麦的正/负极之间分别需要“1uF电容 + 2K电阻”上/下拉滤波，确保HaaS100可以采集到干净清晰的录音信号。具体连接方式参考上图。
- **喇叭**：分为3线（VCC + GND + DIN）和4线（VCC + GND + DIN_P + DIN_N）两种经典的喇叭连接方式。上图中我们采用的是3线制喇叭，和HaaS100的连接参考上图。



<a name="fwStn"></a>
# 2、端测：软件开发


<a name="LpMp4"></a>
## 2.1 AliOS Things开发环境准备

- AliOS Things基础工具：aostools，建议参考《[AliOS Things开发环境一键安装和使用说明](https://gitee.com/alios-things/dev_tool/blob/master/README.md)》快速搭建开发环境。
- VS code IDE：《[使用VS code IDE开发](https://help.aliyun.com/document_detail/184185.html?spm=a2c4g.11186623.6.641.1c691581aewxMd)》
- HaaS100代码获取、编译、烧录：《[HaaS100快速开始](https://help.aliyun.com/document_detail/184184.html?spm=a2c4g.11186623.6.640.7d096528tL9p5P)》
```shell
例如：git命令行从gitee下载源码的方法如下
git clone https://gitee.com/alios-things/AliOS-Things.git -b dev_3.1.0_haas
```


<a name="e8gzv"></a>
## 2.2 源码介绍
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/100557/1608627094820-a70d233b-eb4d-4508-b742-933ecc98194a.png#align=left&display=inline&height=301&margin=%5Bobject%20Object%5D&name=image.png&originHeight=682&originWidth=902&size=233480&status=done&style=none&width=398)<br />图2: 云端钉一体智能语音播放器软件框架<br />
<br />云端钉一体智能语音播放器软件模块包括：

- audio_demo: 主要打通HaaS100和阿里云物联网平台连接、通信，物模型解析，播放器功能逻辑的实现。
- uVoice组件：是AliOS Things上智能语音解决方案的核心组件，提供了本地音频，URL音频，TTS合成等基础功能，音频格式支持mp3, m4a, wav, opus等主流格式。
- A2SA组件：是AliOS Things上音频服务框架，兼容ALSA应用接口访问，支持音频硬件驱动抽象，多音频驱动加载/卸载，VFS接口支持等功能。多播放焦点管理，混音，softvol，重采样等功能设计中。
- 麦克风/喇叭硬件：HaaS100上已经集成了模拟麦和Speaker的硬件驱动。



<a name="f1rSa"></a>
## 2.3 代码编译、固件烧录
编译audio_demo的命令如下
```shell
# 下载音频解码器
cd ./components/uvoice/codec/opensource/pvaac
python get_pv_aac.py
cd ./components/uvoice/codec/opensource/pvmp3
python get_pvmp3.py

# 编译audio_demo
cd ./solution/audio_demo
aos make
```
编译成功之后请参考《[HaaS100快速开始](https://help.aliyun.com/document_detail/184184.html?spm=a2c4g.11186623.6.640.7d096528tL9p5P)》文章将固件更新到HaaS100板子上。最新的版本支持Windows, Mac下刷机。<br />

<a name="SMzxm"></a>
# 3、本地语音播放器调试
基于HaaS100开发的智能音乐播放器支持离线和在线两种调试模式。离线调试模式支持从本地文件系统中播放音频，无需给HaaS100配网，可用于快速验证HaaS100的录音、播放的基本功能是否正常。在线调试模式要求连接阿里云IoT平台，优点是支持云端调试和手机小程序调试能力，且支持TTS语音合成功能。<br />

<a name="fCn7h"></a>
## 3.1 离线音频播放器调试
因为离线音频播放调试需要先在本地文件系统的/data分区中打包音频文件，目前支持.wav和.mp3等常见的音频格式。打包音频文件到/data分区方法如下，
```shell
# 拷贝音频文件到hardware/chip/haas1000/prebuild/data
cp ./test.mp3 hardware/chip/haas1000/prebuild/data

# 重新编译audio_demo
cd ./solution/audio_demo && aos make

# 烧录新编译出来的littlefs.bin和ota_rtos.bin，文件路径位于：./hardware/chip/haas1000/release/release_bin
```
HaaS100固件烧录成功之后可以在串口中敲入“help”命令查看所有支持的CLI调试命令，其中和智能语音播放器相关的常用命令和使用方法如下，
```shell
# 加载HaaS100声卡驱动
串口CLI命令：sound_install_driver

# 循环播放.wav音频10次
sound_wav /data/7.wav 10

# 播放.mp3（只支持1次）
play fs:/data/6.mp3

# 开始录音、播放循环测试
sound_loopback start

# 结束录音、播放循环测试
sound_loopback stop

# 读取当前播放音量（0～100）
sound_getvol

# 设置当前播放音量为90 (0~100)
sound_setvol 90
```


<a name="6c7n7"></a>
## 3.2 在线音频播放器调试
当上述离线音频播放测试通过之后，证明HaaS100的录音、播放器的硬件和软件环境都是OK的了，接下来就可以尝试连接阿里云IoT平台进行在线音频播放器调试。<br />

<a name="qm8oV"></a>
### 3.2.1 WI-FI 配网
HaaS100支持两种WIFI配网方法，分别描述如下，<br />

<a name="3zsJY"></a>
#### 3.2.1.1 修改源码配网
在HaaS100的源码中配置Wi-Fi配网信息，配置好后烧录新固件，HaaS100在开机后会自动连接您配置SSID和Password连接路由器（请务必确认Wi-Fi天线是正常连接的）。首次使用建议使用linkkit_demo例程，直接在application/examples/linkkit_demo/app_entry.c添加下图红框中的代码，并修改为正在使用的SSID和Password。<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/100557/1613814427018-12e7b54c-4138-4b1f-8b04-55c4b596cbf6.png#align=left&display=inline&height=283&margin=%5Bobject%20Object%5D&name=image.png&originHeight=692&originWidth=1456&size=1004780&status=done&style=none&width=596)<br />图8: 源码中修改配网信息<br />

<a name="UbTZq"></a>
#### 3.2.1.2 串口CLI命令行配网
```shell
# 串口输入配网命令
netmgr -t wifi -c {ssid} {password}
```
如果配网信息和四元组信息都正确的情况下，HaaS100和物联网平台连接成功后有如下HardwareError ping-pong日志打印。<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/100557/1608621490069-0ed31cd4-956c-44ab-973d-b02562482354.png#align=left&display=inline&height=273&margin=%5Bobject%20Object%5D&name=image.png&originHeight=546&originWidth=955&size=300874&status=done&style=none&width=477.5)<br />图9: 连接物联网平台成功后的ping-pong日志打印<br />
<br />如果，有些开发者发现自己的HaaS100板子WIFI连接路由器困难，那么可以尝试以下方法排查。

- 更新github上的最新固件
- 串口终端：>> aos_mac WIFI，获取WIFI MAC地址，如果是全0，那么需要更新MAC地址。
- HaaS VIP钉群中咨询@谷饮，获取唯一的WIFI MAC地址。
- 串口终端：>> aos_mac WIFI XX:XX:XX:XX:XX:XX，更新WIFI MAC地址。



<a name="TR1OS"></a>
### 3.2.2 播放在线音乐
当HaaS100配网成功且可以访问外网之后，可以尝试在串口CLI中使用“play”命令播放在线音乐了，
```shell
# 播放在线音乐url, 例如
play https://test-music-url.com

# 其中上面的测试音频url是无效的，因为涉及到潜在的音乐版权问题，需要开发者自己提供可以播放的音乐url。
```


<a name="GQhpF"></a>
### 3.2.3 TTS语音合成
在我们的uvoice组件中集成了阿里巴巴的“[智能语音交互](https://nls-portal.console.aliyun.com/overview)”的部分功能，例如免费试用版的“语音合成”功能，需要商用的用户可以付费开通商用版。<br />用户需要登陆“[智能语音交互](https://nls-portal.console.aliyun.com/overview)”主页，并根据引导创建一个包含“语音合成”功能的项目。项目创建完成后，项目主页中会生成该项目的“Appkey”, 总览主页中可以获取到“AccessToken”两个重要信息。<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/100557/1608619205797-e5582c01-ef9e-448e-b11a-90c2a992712a.png#align=left&display=inline&height=382&margin=%5Bobject%20Object%5D&name=image.png&originHeight=763&originWidth=1914&size=235451&status=done&style=none&width=957)<br />图5: 创建智能语音交互项目和获取AppKey<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/100557/1608619275417-8513d1c8-8a47-4651-ab04-a3543c271fb9.png#align=left&display=inline&height=211&margin=%5Bobject%20Object%5D&name=image.png&originHeight=422&originWidth=1911&size=194112&status=done&style=none&width=955.5)<br />图6: 获取智能语音交互AccessToken<br />
<br />在HaaS100 SDK的components/service/uvoice/test/test_tts.c中，我们需要填写正确的AppKey和AccessToken信息才可以使用阿里巴巴“智能语音交互 - 语音合成”功能。值得注意的是，AccessToken是动态刷新的，周期是每个用户账户可配置的，默认是2天变化一次。因此在功能开发阶段，需要周期性的更新固件中的AccessToken，否则可能出现语音合成功能访问失败的问题。如果是产品量产阶段，可以基于阿里云端一体的安全通道周期更新设备端AccessToken。<br />阿里云智能语音合成功能提供了[Java/ C++/ Restful](https://help.aliyun.com/document_detail/84435.html?spm=a2c4g.11186623.6.609.30837c70z3hUCK)等多种SDK，开发者也可以自己下载SDK适配。<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/100557/1608619535224-42974b3e-c384-4794-9991-3a5baf7f9bdf.png#align=left&display=inline&height=222&margin=%5Bobject%20Object%5D&name=image.png&originHeight=444&originWidth=839&size=214183&status=done&style=none&width=419.5)<br />图7: uvoice组件中修改AppKey和AccessToken代码位置<br />
<br />配置好AppKey和AccessToken之后，参考2.3章节重新编译audio_demo并烧录固件，按以下步骤可以本地测试TTS语音合成功能。
```shell
# TTS合成，生成的语音保存在/data/tts.mp3文件中
tts "今天上海天气晴转多云、气温26摄氏度" /data/tts.mp3
```


<a name="tMLX1"></a>
# 4、云端钉一体语音播放器调试


<a name="0Gr6W"></a>
## 4.1 阿里云IoT平台配置
阿里云物联网平台创建产品、设备的详细过程可以参考文章《[阿里云物联网平台快速入门实践](https://blog.csdn.net/HaaSTech/article/details/111190159)》。<br />

<a name="JwrI6"></a>
### 4.1.1 创建智能语音播放器产品
“云端钉一体的智能语音播放器”物模型设计可以参考：“智能语音播放器物模型”（待发布）。在开发者创建自己产品时，可以选择导入我们提供的“智能语音播放器物模型”来生成一个临时产品，从而快速的验证云端钉一体的智能语音播放器方案，请参考下图步骤。<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/100557/1608628568375-ac7ce5c4-7859-4519-8c81-7948ba53a45b.png#align=left&display=inline&height=360&margin=%5Bobject%20Object%5D&name=image.png&originHeight=720&originWidth=1916&size=224391&status=done&style=none&width=958)<br />图10: 创建产品时快速导入物模型（待发布）<br />

<a name="6NcBY"></a>
### 4.1.2 修改HaaS100四元组信息
在文件application/example/audio_demo/linkkit_example_solo.c中包含了HaaS100设备和阿里云物联网平台链接的关键密钥信息（四元组）。<br />![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/100557/1608616687119-9ca9af17-471a-4730-b0fd-3bde50072382.png#align=left&display=inline&height=209&margin=%5Bobject%20Object%5D&name=image.png&originHeight=242&originWidth=707&size=148257&status=done&style=none&width=611)<br />图3: HaaS100开源SDK中修改audio_demo四元组信息<br />
<br />每位用户应该根据自己在"[阿里云物联网平台](https://iot.console.aliyun.com/product)"中创建的产品信息中可以获取到一下四元组信息。只有在audio_demo/linkkit_example_solo.c中填写了正确的四元组信息，HaaS100语音播放器才能正确连接用户自己账号的阿里云物联网平台。

- "ProductKey"：用户创建的产品类型的Key（唯一ID）
- "ProductSecret"：用户创建的产品类型的密钥（和ProductKey一一对应）
- "DeviceName": 该产品类型下的某个具体设备名字（某个产品类型下可以有很多量产设备）
- "DeviceSecret": 该产品类型下的名字为DeviceName设备的密钥（和DeviceName一一对应）

![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/100557/1608617365393-b014838e-ab73-4adb-9b1b-cbec8e23e53f.png#align=left&display=inline&height=415&margin=%5Bobject%20Object%5D&name=image.png&originHeight=829&originWidth=1822&size=269041&status=done&style=none&width=911)<br />图4: 阿里云物联网平台获取四元组信息页面<br />
<br />**备注**：修改完四元组信息之后，别忘记参考2.3章节重新编译、烧录HaaS100固件。<br />

<a name="37aMt"></a>
## 4.2 钉钉小程序设计


<a name="zgszY"></a>
### 4.2.1 快速开始
文章《[30分钟上手HaaS小程序开发](https://blog.csdn.net/HaaSTech/article/details/110850634)》, 文中介绍了小程序开发工具IDE、HaaS钉钉小程序示例代码获取、钉钉小程序申请、小程序真机调试等基础内容。开发者可以根据这篇文章快速上手一个自己的小程序应用开发。<br />

<a name="XeDzj"></a>
### 4.2.2 小程序：HaaS语音播放器
下载的“HaaS钉钉小程序示例代码”中包含了“HaaS智能语音播放器”示例源码。HaaS语音播放器中创建了2个page，分别是HaasAudioPlayer和playList。

   - HaasAudioPlayer页面功能有：加载/卸载HaaS100声卡，打开/关闭录音，TTS语音合成入口，当前播放音乐的基础信息（歌曲名、歌手名、专辑图片），播放模式选择，音效选择，音量控制，播放控制（播放、暂停、上一首、下一首）以及播放列表入口。
   - playList页面：自动从./mock/data.json中导入用户与设置好的播放列表信息。



<a name="9YvwS"></a>
### 4.2.3 小程序账号信息修改
要成功使用HaaS智能语音播放器小程序，开发者需要正确输入用户的账号信息，包括

   - accessKeyId + accessKeySecret: 钉钉小程序访问用户“阿里云物联网平台”账户的密钥信息。修改代码路径是./pages/app.js。

![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/100557/1608694310623-1c17cc27-9eb4-47e2-a6fd-e77a21be5b66.png#align=left&display=inline&height=87&margin=%5Bobject%20Object%5D&name=image.png&originHeight=174&originWidth=795&size=110301&status=done&style=none&width=397.5)<br />图10: 访问阿里云物联网平台的密钥信息<br />

   - DeviceName + ProductKey：指定钉钉小程序需要控制的用户物联网平台中的某个HaaS100设备。修改代码路径是./pages/HaasAudioPlayer/HaasAudioPlayer.js

![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2020/png/100557/1608694393483-b0ea70aa-0628-493f-be03-8521649d9269.png#align=left&display=inline&height=65&margin=%5Bobject%20Object%5D&name=image.png&originHeight=130&originWidth=840&size=57817&status=done&style=none&width=420)<br />图11: 访问阿里云物联网平台某个HaaS设备的密钥信息<br />

<a name="W2kY7"></a>
### 4.2.4 修改播放列表
开发者可以自己修改歌曲播放列表，配置文件位于./mock/data.json。每首歌曲信息包含: 

   - id: 歌曲在播放列表中的id
   - copyrightId：歌曲版权信息，可忽略
   - picS: 歌曲在播放页面中显示的图片信息
   - songName: 歌曲名
   - singerName: 歌手名
   - url: 歌曲链接，可以是本地文件系统中的歌曲文件，例如fs:/data/6.mp3，也可以是浏览器可以直接播放的网络歌曲链接。



<a name="xGVXl"></a>
# 5、总结
本文提供了“本地”和“云端钉一体”两种调试语音播放器的方式，本地的方式可以快速验证硬件和端测软件的基础功能是否OK，云端钉一体的方式让用户在任何一款手机、平板设备上都可以通过支付宝或者钉钉小程序远程控制HaaS100音乐播放器，更具有可玩性。祝您体验HaaS云端钉一体智能语音播放器方案愉快。

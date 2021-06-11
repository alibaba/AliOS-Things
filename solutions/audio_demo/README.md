@page audio_demo 用钉钉小程序控制HaaS100播放语音示例

[更正文档](https://gitee.com/alios-things/audio_demo/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 1、方案介绍
本文主要介绍如何基于HaaS100硬件平台搭建“云端钉一体”（阿里云IoT平台 + HaaS100 + 钉钉小程序）的智能语音播放器（以下简称智能语音播放器）。该智能语音播放器包含如下功能：

> * 小程序：加载/卸载HaaS100上的声卡模块。
> * 小程序：TTS (Text to Speech)，智能语音合成功能。
> * 小程序：开始/停止录音，录音文件路径/data/rec.pcm
> * 小程序：音乐播放功能，例如音量调节/播放/暂停/上一首/下一首/播放列表等。
> * 本地Shell: 音量调节，本地音乐/ 网络音乐播放（.mp3, .m4a等格式）等。
> * 本地Shell: TTS (Text to Speech)，智能语音合成功能。

# 2、硬件平台搭建
**智能语音播放器整体框架图**

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i1/O1CN018hkdjA1fipAcArks7_!!6000000004041-2-tps-2130-1468.png"  style="max-width:800px;" />
</div>

智能语音播放器需要的硬件模块包括：
**硬件列表**
购买链接仅供参考！！我们不负责商家发货的品质保障等问题！！

| 名称        |   数量   |  参考链接  |
| ------  | :-----:  | :----:  |
| HaaS100开发版 | 1 |   [HaaS100购买链接](https://detail.tmall.com/item.htm?spm=a220m.1000858.1000725.2.38a23a98TnoFxv&id=627354125244&user_id=4160407286&cat_id=2&is_b=1&rn=bb0ed71a917987289b7d66f1803f4efc)     |
| microUSB数据线        |   1   |   普通microusb线即可   |
| 模拟MIC        |    1    |  [模拟MIC参考链接](https://item.taobao.com/item.htm?spm=a230r.1.14.74.43c6552bgcYFUD&id=571778051363&ns=1&abbucket=8#detail)  |
| 喇叭        |   1   |   [喇叭参考链接](https://item.taobao.com/item.htm?spm=a230r.1.14.232.2ec351cbaSvK0N&id=621164763171&ns=1&abbucket=3#detail)   |

# 3、HaaS100端测软件开发
**智能语音播放器的软件框架图**，

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01r7XygD1SJ5uyd0Nys_!!6000000002225-2-tps-902-682.png"  style="max-width:800px;" />
</div>

如上图所示智能语音播放器软件模块包括：
> * audio_demo: 主要打通HaaS100和阿里云物联网平台连接、通信，物模型解析，播放器功能逻辑的实现。
> * uVoice组件：是AliOS Things上智能语音解决方案的核心组件，提供了本地音频，URL音频，TTS合成等基础功能，音频格式支持mp3, m4a, wav, opus等主流格式。
> * A2SA组件：是AliOS Things上音频服务框架，兼容ALSA应用接口访问，支持音频硬件驱动抽象，多音频驱动加载/卸载，VFS接口支持等功能。多播放焦点管理，混音，softvol，重采样等功能设计中。
> * 麦克风/喇叭驱动：HaaS100上已经集成了模拟麦和Speaker的硬件驱动。

HaaS100端测软件开发包含以下3个步骤：
> * AliOS Things开发环境搭建
> * HaaS100智能语音播放器代码下载
> * 代码编译、烧录

## 3.1、AliOS Things开发环境搭建
案例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 3.2、智能语音播放器代码下载
该案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。
其中，
> 选择解决方案: “HaaS云端钉智能语音播放器”或者“audio_demo”
> 选择开发板: HaaS100

## 3.3、打包测试音频文件到文件系统（仅“4.1：播放器离线调试”需要）
因为4.1: 播放器离线调试章节需要测试播放本地文件系统中/data/6.wav 或者 /data/7.mp3音频文件。所以需要提前准备好这两个音频文件，并按下面2个步骤打包音频文件到文件系统中。
```shell
cp ./6.wav ./hardware/chip/haas1000/prebuild/data/  # 拷贝当前目录下的6.wav文件到文件系统打包目录./hardware/chip/haas1000/prebuild/data/下。注意：6.wav文件需要自己准备。
cp ./7.mp3 ./hardware/chip/haas1000/prebuild/data/  # 拷贝当前目录下的7.mp3文件到文件系统打包目录./hardware/chip/haas1000/prebuild/data/下。注意：7.mp3文件需要自己准备。
```

```yaml
# 确认./hardware/chip/haas1000/package.yaml文件中第177行至179行是打开的状态。如下，
program_data_files:
   - filename: release/write_flash_tool/ota_bin/littlefs.bin
     address: 0xB32000
```

## 3.4、代码编译、烧录
-- 固件编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

-- 固件烧录方法可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)。

# 4、功能调试
智能语音播放器支持，
> * 本地Shell命令行调试模式：又分为离线模式和在线模式
> * 云端钉一体调试模式

其中离线调试模式支持从本地文件系统中播放音频，无需给HaaS100配网，可用于快速验证HaaS100的录音、播放的基本功能是否正常。在线调试模式要求HaaS100网络在线，支持网络音频的播放以及阿里巴巴达摩院TTS语音合成功能。

## 4.1、播放器离线调试
HaaS100固件烧录成功之后可以在串口中敲入“help”命令查看所有支持的CLI调试命令，其中和智能语音播放器相关的常用命令和使用方法如下，
```shell
# 查看当前固件支持的所有Shell命令
help

# 加载HaaS100声卡驱动
sound_install_driver

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

## 4.2、播放器在线调试
当上述离线音频播放测试通过之后，证明HaaS100的录音、播放器的硬件和软件环境都是OK的了，接下来就可以尝试连接阿里云IoT平台进行在线音频播放器调试。请按下述步骤操作，

## 4.2.1 WI-FI 配网
HaaS100支持Shell命令行给WIFI配网，方法如下，
```shell
# 串口输入配网命令
netmgr -t wifi -c {ssid} {password}
```
如果配网信息和四元组信息都正确的情况下，HaaS100和阿里云IoT平台连接成功后有如下HardwareError ping-pong日志打印。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01y4cvYs1baLND8fY7R_!!6000000003481-2-tps-955-546.png"  style="max-width:800px;" />
</div>

如果，有些开发者发现自己的HaaS100板子WIFI连接路由器困难，那么可以尝试以下方法排查。
> * 首先参考3.2章节更新最新固件代码
> * 然后通过Shell命令获取WIFI MAC地址,方法如下
> * aos_mac WIFI
> * 如果WIFI MAC是全0，那么需要更新MAC地址。HaaS VIP钉群中咨询@谷饮，获取唯一的WIFI MAC地址。
> * 最后更新获取到的WIFI MAC，方法如下
> * aos_mac WIFI XX:XX:XX:XX:XX:XX

## 4.2.2 播放在线音乐
当HaaS100配网成功且可以访问外网之后，可以尝试在串口Shell中使用“play”命令播放在线音乐了，
```shell
# 播放在线音乐url, 例如
play https://test-music-url.com

# 其中上面的测试音频url是无效的，因为涉及到潜在的音乐版权问题，需要开发者自己提供可以播放的音乐url。
```

## 4.2.3 TTS语音合成
在我们的uvoice组件中集成了阿里巴巴的“[智能语音交互](https://nls-portal.console.aliyun.com/overview)”的部分功能，您可以尝试免费试用版的“语音合成”功能，需要商用的用户也可以选择付费开通商用版。阿里云智能语音合成功能提供了[Java/ C++/ Restful](https://help.aliyun.com/document_detail/84435.html?spm=a2c4g.11186623.6.609.30837c70z3hUCK)等多种SDK，开发者也可以自己下载SDK适配。

用户首先登陆“[智能语音交互](https://nls-portal.console.aliyun.com/overview)”主页，并根据引导创建一个包含“语音合成”功能的项目。项目创建完成后，项目主页中会生成该项目的“Appkey”和“AccessToken”两个重要信息。

**创建智能语音交互项目和获取AppKey**

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01BrEHmM254gTETXgHt_!!6000000007473-2-tps-1914-763.png"  style="max-width:800px;" />
</div>

**获取智能语音交互AccessToken**

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN017yFPVh1tgRjqtfWJX_!!6000000005931-2-tps-1911-422.png"  style="max-width:800px;" />
</div>

在HaaS100 SDK的components/service/uvoice/test/test_tts.c中，我们需要填写正确的AppKey和AccessToken信息才可以使用阿里巴巴“智能语音交互 - 语音合成”功能。值得注意的是，AccessToken是动态刷新的，周期是每个用户账户可配置的，默认是2天变化一次。因此在功能开发阶段，需要周期性的更新固件中的AccessToken，否则可能出现语音合成功能访问失败的问题。如果是产品量产阶段，可以基于阿里云端一体的安全通道周期更新设备端AccessToken。

**uvoice组件中修改AppKey和AccessToken代码位置**

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN014Nynrb1GsZCiTJuqT_!!6000000000678-2-tps-839-444.png"  style="max-width:800px;" />
</div>

配置好AppKey和AccessToken之后，参考上述编译和烧录，按以下步骤可以本地测试TTS语音合成功能。
```shell
# TTS合成，生成的语音保存在/data/tts.mp3文件中
tts "今天上海天气晴转多云、气温26摄氏度" /data/tts.mp3
```

## 4.3 云端钉一体调试
智能语音播放器的云端钉一体调试模式包含：阿里云IoT平台配置，以及钉钉小程序开发两大部分。

## 4.3.1 阿里云IoT平台配置
阿里云物联网平台创建产品、设备的详细过程可以参考文章《[阿里云物联网平台快速入门实践](https://blog.csdn.net/HaaSTech/article/details/111190159)》

### 4.3.1.1 创建智能语音播放器产品
“云端钉一体的智能语音播放器”物模型设计可以参考：“智能语音播放器物模型”（待发布）。在开发者创建自己产品时，可以选择导入我们提供的“智能语音播放器物模型”来生成一个临时产品，从而快速的验证云端钉一体的智能语音播放器方案，请参考下图步骤。

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01CudBAj26Zes9vPKnC_!!6000000007676-2-tps-1916-720.png"  style="max-width:800px;" />
</div>

### 4.3.1.2 修改HaaS100三元组信息
用户可以在"[阿里云物联网平台](https://iot.console.aliyun.com/product)"中创建的产品信息中可以获取到三元组信息，只有正确配置了HaaS100固件中的三元组信息，HaaS100语音播放器才能正确连接用户自己账号的阿里云物联网平台。修改四元组的位置在文件./solutions/audio_demo/data_model_basic_demo.c的demo_main()中，代码示例如下，

```c
/* TODO: 替换为自己设备的三元组 */
char *product_key = 用户创建的产品类型的Key（唯一ID）
char *device_name = 该产品类型下的某个具体设备名字（某个产品类型下可以有很多量产设备）
char *device_secret = 该产品类型下的名字为DeviceName设备的密钥（和DeviceName一一对应）

```

阿里云物联网平台获取三元组信息页面:

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i3/O1CN01VpR3zn1uFiEa9uGtP_!!6000000006008-2-tps-1822-829.png"  style="max-width:800px;" />
</div>

**备注**：修改完三元组信息之后，别忘记参考2.3章节重新编译、烧录HaaS100固件。

## 4.3.2 钉钉小程序设计
文章《[30分钟上手HaaS小程序开发](https://blog.csdn.net/HaaSTech/article/details/110850634)》, 文中介绍了小程序开发工具IDE、HaaS钉钉小程序示例代码获取、钉钉小程序申请、小程序真机调试等基础内容。开发者可以根据这篇文章快速上手一个自己的小程序应用开发。

下载的“HaaS钉钉小程序示例代码”中包含了“HaaS智能语音播放器”示例源码(开源披露流程中，敬请期待)。HaaS语音播放器中创建了2个page，分别是HaasAudioPlayer和playList。
> * HaasAudioPlayer页面功能有：加载/卸载HaaS100声卡，打开/关闭录音，TTS语音合成入口，当前播放音乐的基础信息（歌曲名、歌手名、专辑图片），播放模式选择，音效选择，音量控制，播放控制（播放、暂停、上一首、下一首）以及播放列表入口。
> * playList页面：自动从./mock/data.json中导入用户与设置好的播放列表信息。

要成功使用HaaS智能语音播放器小程序，开发者需要正确输入用户以下用户信息，
## 4.3.2.1 accessKey信息
> * accessKeyId
> * accessKeySecret

这是钉钉小程序访问用户“阿里云物联网平台”账户的密钥信息，修改代码路径是./pages/app.js。
**密钥信息修改位置参考**

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i2/O1CN01ocLqjB1lyIFEjtKpB_!!6000000004887-2-tps-795-174.png"  style="max-width:800px;" />
</div>

## 4.3.2.2 产品信息
> * DeviceName
> * ProductKey

这是钉钉小程序需要控制的用户物联网平台中的某个HaaS100设备。修改代码路径是./pages/HaasAudioPlayer/HaasAudioPlayer.js。
**HaaS设备的密钥信息修改位置参考**

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01NB8Xqa1PCHAtWeFFB_!!6000000001804-2-tps-840-130.png"  style="max-width:800px;" />
</div>

## 4.3.2.3 修改播放列表
开发者可以自己修改歌曲播放列表，配置文件位于./mock/data.json。每首歌曲信息包含: 
> * id: 歌曲在播放列表中的id
> * copyrightId：歌曲版权信息，可忽略
> * picS: 歌曲在播放页面中显示的图片信息
> * songName: 歌曲名
> * singerName: 歌手名
> * url: 歌曲链接，可以是本地文件系统中的歌曲文件，例如fs:/data/6.mp3，也可以是浏览器可以直接播放的网络歌曲链接。

# 5、总结
本文提供了“本地”和“云端钉一体”两种调试语音播放器的方式，本地的方式可以快速验证硬件和端测软件的基础功能是否OK，云端钉一体的方式让用户在任何一款手机、平板设备上都可以通过支付宝或者钉钉小程序远程控制HaaS100音乐播放器，更具有可玩性。祝您体验HaaS云端钉一体智能语音播放器方案愉快。

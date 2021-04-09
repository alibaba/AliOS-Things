@page ucloud_ai_demo ucloud_ai_demo

# 1. 案例简介
ucloud_ai_demo是基于云端AI能力实现的AI识别案例，主要有三个部分组成：
1. 通过采集WiFi摄像头(ESP32)的数据后得到jpeg图像;
2. 上传至云端OSS，再通过ucloud_ai组件实现视觉智能开放平台对图片识别处理；
3. 通过LCD屏进行画面显示及识别结果显示。

该案例中支持了15种AI能力，通过在aiconfig.h中配置AI_MODEL选择对应的AI模型。所有的AI模型类型在aiagent_common.h中定义。


# 2. 基础知识
## 2.1 基础目录结构
```sh
.
├── helloworld.c   # 该solution核心打印输出代码，入口**application_start**
├── k_app_config.h # 内核组件的配置开关，优先级低于**k_config.h**
├── maintask.c     # 系统主任务入口处理，入口**aos_maintask**
├── Makefile       # aos make编译时入口
├── package.yaml   # 编译系统配置文件
└── SConstruct     # Makefile => Scon => aostools

├── linkkit_event.c     # 连接物联网平台时使用的接口
├── main.c              # demo应用主程序入口，入口**application_start**
├── maintask.c          # 系统主任务入口处理，入口**aos_maintask**
├── Makefile            # aos make编译时入口
├── package.yaml        # 编译系统配置文件
├── SConstruct          # Makefile => Scon => aostools
└── ucloud_ai_demo.c    # AI识别处理主程序
```

# 3. 方案组成
整个方案由HaaS100、WiFi摄像头、LCD组成。LCD与HaaS100通过SPI连接，HaaS100通过Http请求获取到JPEG数据最终显示到LCD上。

<div align=left display=flex>
    <img src="https://img-blog.csdnimg.cn/img_convert/ac7f6d577ee932d49790b0e3970b0ae8.png" style="max-width:90%;" />
</div>

## 3.1 WiFi摄像头安装
### 3.1.1 WiFi摄像头选型
市面上的WiFi摄像头比较多，在本例中WiFi摄像头采用ESP官方的ESP32-EYE进行适配，ESP32-CAM是ESP32第三方厂商开发的一款低成本方案，应用也比较广泛，开发者也可以选择它作为方案之一，万能的淘宝上有很多卖家，商家也会提供相应的资料，开发者可以根据自己需要进行调试，购买链接如下：
ESP32-EYE: [https://detail.tmall.com/item.htm?spm=a230r.1.14.1.150d6a6ftZ6h4K&id=611790371635&ns=1&abbucket=3](https://detail.tmall.com/item.htm?spm=a230r.1.14.1.150d6a6ftZ6h4K&id=611790371635&ns=1&abbucket=3)

ESP32-CAM: [https://detail.tmall.com/item.htm?spm=a230r.1.14.1.3f543b21XaGDay&id=581256720864&ns=1&abbucket=3](https://detail.tmall.com/item.htm?spm=a230r.1.14.1.3f543b21XaGDay&id=581256720864&ns=1&abbucket=3)

[https://item.taobao.com/item.htm?spm=a230r.1.14.33.150d6a6ftZ6h4K&id=586201030146&ns=1&abbucket=3#detail](https://item.taobao.com/item.htm?spm=a230r.1.14.33.150d6a6ftZ6h4K&id=586201030146&ns=1&abbucket=3#detail)


### 3.1.2 ESP32-EYE开发配置

#### 3.1.2.1 代码下载
```bash
$git clone --recursive https://github.com/espressif/esp-who.git
```
#### 3.1.2.2 Python环境创建
> 这一个步骤不是必须的，不过如果你有多个python环境的需求，也安装过conda可以使用该步骤为esp32的开发创建一个独立的python开发环境，避免不同开发环境的相互影响，这里也可以参考[《**VSCode中搭建Python虚拟环境SOP**》]https://blog.csdn.net/HaaSTech/article/details/113512377)。
```bash
$conda create -n esp32 python=3.8
```
#### 3.1.2.3 ESP-IDF安装
不同的操作系统安装的步骤也有所差异，请参考官网文档进行安装：
[https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html#get-started-set-up-env](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html#get-started-set-up-env)

#### 3.1.2.4 环境变量设置
> 这里以Macbook为例进行环境变量设置：
```bash
$cd ~/esp/esp-idf
$./install.sh
$. ./export.sh
```
注意：
每次重启终端后都需要执行该步骤，否则找不到idf.py命令，或者可以加入到根目录.bashrc中不用每次再输入该命令。

#### 3.2.1.8 ESP32 EYE网络设置
> SoftAP模式

默认启动后ESP32 EYE已经开启了SSID为ESP32-Camera的AP，可以使用电脑连接该AP。

<div align=left display=flex>
    <img src="https://img-blog.csdnimg.cn/img_convert/d0078a4e4bfb521beb04291497d94970.png" style="max-width:90%;" />
</div>

也可以通过修改sdkconfig来改变ssid/password、station连接数量、AP信道、服务器IP等，然后重新进行编译：
#### ![image.png](https://img-blog.csdnimg.cn/img_convert/bea6f1ff0946804d77c5a01e3d59271c.png)

> Station模式

ESP32也支持station与SoftAP模式共存，比如想让ESP32 EYE接入到SSID为haas_test的局域网中，修改sdkconfig中的ssid/password即可。

<div align=left display=flex>
    <img src="https://img-blog.csdnimg.cn/img_convert/1101892b41017e3f74b6ec258605b890.png" style="max-width:90%;" />
</div>

#### 3.2.1.9 分辨率配置
因为本案例中使用的LCD是320x240的，摄像头采集的画面也相应的设置为QVGA(320x240)减少传输带宽占用，esp-who/examples/single_chip/camera_web_server/main/app_httpd.c中添加代码：
```bash

static esp_err_t capture_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    int64_t fr_start = esp_timer_get_time();

    /*set resolution*/
    sensor_t *sensor = esp_camera_sensor_get();
    sensor->set_framesize(sensor, (framesize_t)5);/*QVGA: 320 x 240*/
    if (res == 0) {
        app_mdns_update_framesize(5);/*QVGA*/
    }
    ......
}
```

#### 3.1.2.5 代码编译
ESP32-EYE的代码中提供了多个Demo，使用camera_web_server来建立一个web服务器，该Demo中摄像头采集的数据以jpeg格式提供，并且提供了以http请求的方式获取jpeg图像数据。编译需要进入到Demo的目录中：
```bash
$cd examples/single_chip/camera_web_server/
$idf.py build
```
#### 3.2.1.6 代码烧录
```bash
$idf.py -p [port] flash
```
例如：
idf.py -p /dev/cu.SLAB_USBtoUART flash
#### 3.2.1.7 Log监视器
查看串口log，进入到camera_web_server所在目录执行。
```bash
$idf.py -p [port] monitor
```
例如：
idf.py -p /dev/cu.SLAB_USBtoUART monitor

<div align=left display=flex>
    <img src="https://img-blog.csdnimg.cn/img_convert/cb573d5a42e695269a675ebc5be96c0d.png" style="max-width:90%;" />
</div>

所以camera wifi的IP就是192.168.3.135。

#### 3.2.1.9 检查摄像头画面采集
为了确认ESP32-EYE摄像头是否正常，电脑连接ESP32-EYE的WiFi网络ESP32-Camera，先通过电脑方式查看web界面http://192.168.4.1:80/capture：
抓取当前画面http://192.168.4.1:80/capture：

<div align=left display=flex>
    <img src="https://img-blog.csdnimg.cn/20210127165159696.png" style="max-width:90%;" />
</div>

### 3.2 LCD连线
LCD购买链接[https://item.taobao.com/item.htm?spm=a1z09.2.0.0.768d2e8d9D3S7s&id=38842179442&_u=m1tg6s6048c2](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.768d2e8d9D3S7s&id=38842179442&_u=m1tg6s6048c2)

请选择购买2.4寸屏。

HaaS100扩展口：

<div align=left display=flex>
    <img src="https://img-blog.csdnimg.cn/img_convert/b18b27ec7957e010d0380e76ebb33e98.png" style="max-width:90%;" />
</div>

LCD与HaaS100接线对应pin脚：

<div align=left display=flex>
    <img src="https://img-blog.csdnimg.cn/img_convert/5b2c8c014991f028978f5743ba22d193.png" style="max-width:90%;" />
</div>

# 4. Demo体验

## 4.1 云端功能开通
> 登陆https://vision.aliyun.com免费开通如下功能：
```sh
人脸人体: https://vision.aliyun.com/facebody
文字识别: https://vision.aliyun.com/ocr
分割抠图: https://vision.aliyun.com/imageseg
目标检测: https://vision.aliyun.com/objectdet

如没有阿里云账号，请登陆http://www.aliyun.com开通
```

> 登陆oss.console.aliyun.com创建bucket，注意：
```sh
1. bucket名称为小写
1. 创建Bucket时地域选择“上海”
2. 读写权限选择“公共读”
```

## 4.2 配置OSS信息
在solutions/ucloud_ai_demo/package.yaml中配置：
```sh
OSS_ACCESS_KEY "Your-Access-Key"
OSS_ACCESS_SECRET "Your-Access-Secret"
OSS_ENDPOINT "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET "Your-OSS-Bucket"

KEY以及SECRET获取链接: https://usercenter.console.aliyun.com/#/accesskey
ENDPOINT使用默认即可，BUCKET请使用你创建好的Bucket名称
```
## 4.3 AI模型配置
在components/ai_agent/include/aiconfig.h中配置：
```sh
# 配置AI模型为人脸比对
#define AI_MODEL AI_MODEL_COMPARING_FACEBODY
```
默认是人脸比对。

## 4.4 配置人脸原始对比图像
登陆http://oss.console.aliyun.com 上传你的人脸到4.1中创建的bucket中，并复制路径到：
```sh
MYFACE_PATH: "http://viapi-test.oss-cn-shanghai.aliyuncs.com/viapi-3.0domepic/facebody/CompareFace/CompareFace-left1.png"
```

## 4.5 WiFi摄像头IP配置
wifi camera的http访问地址,用户根据自己摄像头的IP地址进行替换：
在components/ucamera/package.yaml中配置：
```sh
  WIFICAMERA_URL: "http://192.168.43.166:80/capture"
```
> 192.168.43.166替换为3.2.1.7中从log获取的WiFi摄像头IP。


## 4.6 AliOS Things开发环境搭建
开发环境的搭建请参考 @ref HaaS100_Quick_Start (搭建开发环境章节)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。

## 4.7 智能语音播放器代码下载
云端AI识别的代码下载请参考 @ref HaaS100_Quick_Start (创建工程章节)，其中：
> 选择解决方案: “云端AI案例”或者“ucloud_ai_demo”
> 选择开发板: HaaS100

## 4.8 代码编译、烧录
参考 @ref HaaS100_Quick_Start (3.1 编译工程章节)，点击 ✅ 即可完成编译固件。

### 4.8.1 文件件系统烧录
本组件例子中使用到到图片及字体分别存放在代码中hardware/chip/haas1000/prebuild/data/目录下ai_demo_image及font目录，除烧录ucloud_ai_demo image外，需烧录littlefs文件系统，请将hardware/chip/haas1000/package.yaml文件中以下代码段的注释打开：

```yaml
  program_data_files:
    - filename: release/write_flash_tool/ota_bin/littlefs.bin
      address: 0xB32000
```

参考 @ref HaaS100_Quick_Start (3.2 烧录镜像章节)，点击 "⚡️" 即可完成烧录固件。

## 4.9 网络连接
因为HaaS100开发板需要连接到云端，因此需要连接到一个可以上外网的路由器，WiFi摄像头(ESP32-EYE)也只能使用Station模式连接到同一台路由器。

```sh
# 系统起来后在串口输入配网命令
netmgr -t wifi -c {ssid} {password}
```
请将ssid修改为您路由器的WiFi名称，paasword填入路由器的WiFi密码。

## 4.10 识别结果响应
识别到后输出置信度的值，人脸位置以及“boss is coming”字样：

<div align=left display=flex>
    <img src="https://img-blog.csdnimg.cn/img_convert/06a60d5c4ac2ef19bc06a0ac62c76fda.png" style="max-width:90%;" />
</div>

### 4.10.1 字幕提醒
在HaaS 100的扩展屏上显示：
![HaaS 100 LCD 1.gif](https://img-blog.csdnimg.cn/img_convert/f62c4a0057d7c30069d51436e6dbf3cd.gif)

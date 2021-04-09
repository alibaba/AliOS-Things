@page ucamera ucamera

# 概述
ucamera组件是摄像头图像数据处理中间框架，支持不同类型摄像头数据接入，目前已接入wifi摄像头，在使用该组件时，需要使用netmgr网络组件连接网络。

## 版权信息
> Apache 2.0 License

## 目录结构
```sh
.
├── include
│   ├── ucamera_common.h                 # 对外头文件
│   ├── ucamera_device.h                 # 摄像头device相关头文件定义
│   ├── ucamera_service.h                # 摄像头service相关头文件定义
│   └── device
│        └── wifi
│             ├── wifi_camera.h           # wifi摄像头device注册函数头文件
│             └── dechunk.h
├── package.yaml                          # 编译配置文件
└── src
    ├── ucamera_device.c                  # 摄像头设备相关函数实现
    ├── ucamera_service.c                 # 摄像头服务相关函数实现
    ├── example
    │    └── ucamera_example.c            # 使用示例代码
    └── device
         └── wifi
              ├── wifi_camera.c           # wifi摄像头device注册函数实现
              └── dechunk
                    └── dechunk.c         # http数据解析代码实现
```

## 依赖组件

* http

# 常用配置

# API说明
@ref ucamera_aos_api

# 使用示例
示例代码参考example/ucamera_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> ucamera组件的package.yaml中添加
```C
source_file:
  - src/example/ucamera_example.c # add ucamera_example.c
```

## app中添加ucamera组件
> helloworld_demo组件的package.yaml中添加
```C
depends:
  - ucamera: dev_aos # helloworld_demo中引入ucamera组件
  - netmgr: dev_aos  # helloworld_demo中引入netmgr组件
```

## 代码编译、烧录
参考 @ref HaaS100_Quick_Start (3.1 编译工程章节)，点击 ✅ 即可完成编译固件。

### 文件件系统烧录
本组件例子中使用到文件系统存储图片，需烧录littlefs文件系统，请将hardware/chip/haas1000/package.yaml文件中以下代码段的注释打开：

```yaml
  program_data_files:
    - filename: release/write_flash_tool/ota_bin/littlefs.bin
      address: 0xB32000
```

参考 @ref HaaS100_Quick_Start (3.2 烧录镜像章节)，点击 "⚡️" 即可完成烧录固件。


## ucamera示例测试
> ucamera测试依赖WiFi摄像头，本案例测试使用ESP32-EYE进行测试。

### ESP32-EYE开发配置

#### 代码下载
```bash
$git clone --recursive https://github.com/espressif/esp-who.git
```
#### Python环境创建

这一个步骤不是必须的，不过如果你有多个python环境的需求，也安装过conda可以使用该步骤为esp32的开发创建一个独立的python开发环境，避免不同开发环境的相互影响，这里也可以参考[《**VSCode中搭建Python虚拟环境SOP**》]https://blog.csdn.net/HaaSTech/article/details/113512377)。
```bash
$conda create -n esp32 python=3.8
```
#### ESP-IDF安装
不同的操作系统安装的步骤也有所差异，请参考官网文档进行安装：
[https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html#get-started-set-up-env](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html#get-started-set-up-env)

#### 环境变量设置
这里以Macbook为例进行环境变量设置：
```bash
$cd ~/esp/esp-idf
$./install.sh
¥. ./export.sh
```
注意：
每次重启终端后都需要执行该步骤，否则找不到idf.py命令，或者可以加入到根目录.bashrc中不用每次再输入该命令。

#### ESP32 EYE网络设置
> SoftAP模式

默认启动后ESP32 EYE已经开启了SSID为ESP32-Camera的AP，可以使用电脑连接该AP。
![image.png](https://img-blog.csdnimg.cn/img_convert/d0078a4e4bfb521beb04291497d94970.png)
也可以通过修改sdkconfig来改变ssid/password、station连接数量、AP信道、服务器IP等，然后重新进行编译：
#### ![image.png](https://img-blog.csdnimg.cn/img_convert/bea6f1ff0946804d77c5a01e3d59271c.png)

#### 分辨率配置
> 因为本案例中使用的LCD是320x240的，摄像头采集的画面也相应的设置为QVGA(320x240)减少传输带宽占用，esp-who/examples/single_chip/camera_web_server/main/app_httpd.c中添加代码：
```bash

static esp_err_t capture_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    int64_t fr_start = esp_timer_get_time();

    /*set resolution*/
    sensor_t *sensor = esp_camera_sensor_get();
    sensor->set_framesize(sensor, (framesize_t)5);/*QVGA: 3220 x 240*/
    if (res == 0) {
        app_mdns_update_framesize(5);/*QVGA*/
    }
    ......
}
```

#### 代码编译
> ESP32-EYE的代码中提供了多个Demo，使用camera_web_server来建立一个web服务器，该Demo中摄像头采集的数据以jpeg格式提供，并且提供了以http请求的方式获取jpeg图像数据。编译需要进入到Demo的目录中：
```bash
$cd examples/single_chip/camera_web_server/
$idf.py build
```
#### 代码烧录
```bash
$idf.py -p [port] flash
```
例如：
idf.py -p /dev/cu.SLAB_USBtoUART flash
#### Log监视器
查看串口log，进入到camera_web_server所在目录执行。
```bash
$idf.py -p [port] monitor
```
例如：
idf.py -p /dev/cu.SLAB_USBtoUART monitor

#### 检查摄像头画面采集
> 为了确认ESP32-EYE摄像头是否正常，电脑连接ESP32-EYE的WiFi网络ESP32-Camera，先通过电脑方式查看web界面http://192.168.4.1:80/capture：
抓取当前画面http://192.168.4.1:80/capture：
![Pasted Graphic.tiff](https://img-blog.csdnimg.cn/20210127165159696.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0hhYVNUZWNo,size_16,color_FFFFFF,t_70)

## ai_agent示例测试

测试步骤：
```sh
$ ucamera init # 初始化ucamera
$ netmgr -t wifi -c ESP32-Camera  #  连接WiFi摄像头
$ ucamera -t wifi # 获取一帧画面
```

### CLI命令行输入：
```sh
ucamera init #在执行下面的测试命令前，该命令需要优先执行，仅需执行一次即可。
```

> CLI关键日志：
```sh
ucamera service init ok!
```

### CLI命令行输入：
```sh
ucamera -t wifi
```

> CLI关键日志：
```sh
ucamera get frame OK!  #测试结果正常确认(说明从wifi摄像头获取到图像数据)
```

# FAQ

@page ucamera ucamera

**[更正文档](https://gitee.com/alios-things/ucamera/edit/master/README.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

# 概述
ucamera组件是摄像头图像数据处理中间框架，支持不同类型摄像头数据接入，目前已接入wifi摄像头，在使用该组件时，需要使用netmgr网络组件连接网络。

## 版权信息
> Apache 2.0 License

## 目录结构
```tree
├── include
│   ├── ucamera_common.h                 # 对外头文件
│   ├── ucamera_device.h                 # 摄像头device相关头文件定义
│   ├── ucamera_service.h                # 摄像头service相关头文件定义
│   └── device
│        └── wifi
│             ├── wifi_camera.h           # wifi摄像头device注册函数头文件
│             └── dechunk.h
├── package.yaml                          # 编译配置文件
├── src
│   ├── ucamera_device.c                  # 摄像头设备相关函数实现
│   ├── ucamera_service.c                 # 摄像头服务相关函数实现
│   └── device
│        └── wifi
│             ├── wifi_camera.c           # wifi摄像头device注册函数实现
│             └── dechunk
│                   └── dechunk.c         # http数据解析代码实现
└── example
    └── ucamera_example.c                 # 测试用例
```

## 依赖组件

* http

# 常用配置
```yaml
def_config:                              # 组件的可配置项
  CONFIG_WIFI_CAMERA_ENABLE: 1           # 使能WiFi摄像头
```

# API说明
- 参考 [ucamera_aos_api](https://g.alicdn.com/alios-things-3.3/doc/group__ucamera__aos__api.html)


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
  - ucamera: master # helloworld_demo中引入ucamera组件
  - netmgr: master  # helloworld_demo中引入netmgr组件

def_config:
    CLI_IOBOX_ENABLE: 1        # 使能文件系统测试命令，例如ls, ll, mkdir, touch, echo等

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install ucamera

```

上述命令执行成功后，组件源码则被下载到了./components/ucamera路径中。

## 步骤4 添加示例

在ucamera组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/ucamera/tree/master/example)：

```yaml
source_file:
  - "example/ucamera_example.c" # add ucamera_example.c

```
### WiFi摄像头安装
ucamera测试依赖WiFi摄像头，本案例测试使用ESP32-EYE进行测试。
ESP32-EYE购买链接: [https://detail.tmall.com/item.htm?spm=a230r.1.14.1.150d6a6ftZ6h4K&id=611790371635&ns=1&abbucket=3](https://detail.tmall.com/item.htm?spm=a230r.1.14.1.150d6a6ftZ6h4K&id=611790371635&ns=1&abbucket=3)

#### 1. 代码下载
```bash
$git clone --recursive https://github.com/espressif/esp-who.git
```

#### 2. ESP-IDF安装
不同的操作系统安装的步骤也有所差异，请参考官网文档进行安装：
[https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html#get-started-set-up-env](https://docs.espressif.com/projects/esp-idf/zh_CN/latest/esp32/get-started/index.html#get-started-set-up-env)

#### 3. 环境变量设置
这里以Macbook为例进行环境变量设置：

```shell
$ cd ~/esp/esp-idf
$ ./install.sh
$ . ./export.sh
```

注意：
每次重启终端后都需要执行该步骤，否则找不到idf.py命令，或者可以加入到根目录.bashrc中不用每次再输入该命令。

#### 4. ESP32 EYE网络设置
**SoftAP模式**

默认启动后ESP32 EYE已经开启了SSID为ESP32-Camera的AP，可以使用电脑连接该AP。
![image.png](https://img-blog.csdnimg.cn/img_convert/d0078a4e4bfb521beb04291497d94970.png)
也可以通过修改sdkconfig来改变ssid/password、station连接数量、AP信道、服务器IP等，然后重新进行编译：
#### ![image.png](https://img-blog.csdnimg.cn/img_convert/bea6f1ff0946804d77c5a01e3d59271c.png)

#### 5. 分辨率配置
> 为了减少传输带宽占用，摄像头采集的画面设置为QVGA(分辨率320x240)，esp-who/examples/single_chip/camera_web_server/main/app_httpd.c中添加代码：
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

#### 6. 代码编译
使用camera_web_server来建立一个web服务器，该Demo中摄像头采集的数据以jpeg格式提供，并且提供了以http请求的方式获取jpeg图像数据。编译需要进入到Demo的目录中：
```shell
$cd examples/single_chip/camera_web_server/
$idf.py build
```
#### 7. 代码烧录
```bash
$idf.py -p [port] flash
```
例如：
idf.py -p /dev/cu.SLAB_USBtoUART flash

#### 8. Log监视器
查看串口log，进入到camera_web_server所在目录执行。
```bash
$idf.py -p [port] monitor
```
例如：
idf.py -p /dev/cu.SLAB_USBtoUART monitor

#### 9. 检查摄像头画面采集
为了确认ESP32-EYE摄像头是否正常，电脑连接ESP32-EYE的WiFi网络ESP32-Camera，先通过电脑方式查看web界面http://192.168.4.1:80/capture：
抓取当前画面http://192.168.4.1:80/capture：
![Pasted Graphic.tiff](https://img-blog.csdnimg.cn/20210127165159696.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0hhYVNUZWNo,size_16,color_FFFFFF,t_70)

### 配置WiFi摄像头IP
在ucamera/package.yaml中配置WiFi摄像头URL：
```yaml
  WIFICAMERA_URL: "http://192.168.4.1:80/capture"
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

### 文件系统烧录
本组件示例在抓取图像后保存图像到littlefs文件系统，需烧录littlefs文件系统，请将hardware/chip/haas1000/package.yaml文件中以下代码段的注释打开：

```yaml
  program_data_files:
    - filename: release/write_flash_tool/ota_bin/littlefs.bin
      address: 0xB32000
```

上述步骤执行后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

测试步骤：
```shell
$ ucamera init                    # 初始化ucamera
$ netmgr -t wifi -c ESP32-Camera  # 连接WiFi摄像头
$ ucamera -t wifi                 # 获取一帧画面
$ ls /data                        # 查看是否有capture.jpg文件生成
```

**CLI命令行输入：**
```shell
ucamera init # 在执行下面的测试命令前，该命令需要优先执行，仅需执行一次即可。
```

> CLI关键日志：
```shell
ucamera comp init successfully!
```

**CLI命令行输入：**
```shell
ucamera -t wifi
```

> CLI关键日志：
```shell
ucamera get frame OK!  # 测试结果正常确认(说明从wifi摄像头获取到图像数据)
save image to /data/capture.jpg successfully!
```

**CLI命令行输入：**
```shell
ls /data
```

> CLI关键日志：
```shell
capture.jpg  # 生成的文件
```

# FAQ
NA

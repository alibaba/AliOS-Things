@page oled oled

[更正文档](https://gitee.com/alios-things/sh1106/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
OLED，即有机发光二极管（Organic Light-Emitting Diode）。OLED 由于同时具备自发光，不需背光源、对比度高、厚度薄、视角广、反应速度快、可用于挠曲性面板、使用温度范围广、构造及制程较简单等优异之特性。
与LCD相比，LCD需要背光，同样的显示，OLED 效果要来得好一些，OLED分辨率可以做到很高。
本组件是为1.3寸OLED屏驱动IC-SSD1106（与0.96寸OLED稍有不同，SSD1306），SSD1106点阵132*64，SSD1306点阵128*64，都是8页的显存，SSD1106每页132字节，SSD1306每页128字节。
下面看一下其对应关系（SSD1106 1.3寸OLED为例）
![](https://img.alicdn.com/imgextra/i2/O1CN01mlGbcq1LHXFq3FGkJ_!!6000000001274-2-tps-1362-656.png)
与HaaS100的连接
![](https://img.alicdn.com/imgextra/i3/O1CN01RRxCJH1zqsS4JH5zh_!!6000000006766-2-tps-1118-582.png)
## 版权信息


> Apache license v2.0



## 目录结构


```tree
├── src
│   └── sh1106.c          # 基于sh1106 OLED组件服务
├── include
│   ├── front.h           # 字模库文件。
│   ├── icon.h            # 声卡pcm设备驱动，例如录音、播放等。
│   ├── sh1106.h          # 基于sh1106 OLED组件服务API。
│   └── sh1106_hdconfig.h # sh1106硬件相关配置
├── package.yaml          # 编译配置文件
└── example
    └── sh1106_example.c  # oled显示测试代码
```


## 依赖组件


- rhino



# 常用配置


系统中相关配置已有默认值，如需修改配置，统一在sh1106_hdconfig.h修改，具体如下：


> 选择访问sh1106芯片的模式，设置为1，支持软件模拟SPI；设置为0，支持硬件SPI访问：



```yaml
#define USE_SOFT_SPI 1
```


> 选择是否支持录屏功能，设置为0，录屏功能关闭；设置为1，录屏功能打开：



```c
#define RECORD_SCREEN   0
```


> 是否支持fb框架，设置为0，不支持fb框架，支持输出显示；设置为1，支持fb框架：

```c
#define FB_FRAME_EN     0
```
# API说明

> sh1106初始化API接口

```c
uint8_t sh1106_init(void);
```
> sh1106 OLED屏幕显示API接口
> (x,y)是显示的坐标，*p是字符串的首地址，size是字符点集大小(12/16/24) ,mode(0-反显 1-正显)

```c
void sh1106_show_string(uint8_t x, uint8_t y, const uint8_t *p, uint8_t size, uint8_t mode);
```
# 使用示例


组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 app中添加sh1106组件

> helloworld_demo组件的package.yaml中添加

```yaml
depends:
  - sh1106: master # helloworld_demo中引入sh1106组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install sh1106

```

上述命令执行成功后，组件源码则被下载到了./components/drivers/external_device路径中。


## 步骤4 添加示例代码

> sh1106组件的package.yaml中添加example示例代码

```yaml
source_file:
  - src/sh1106.c
  - example/sh1106_example.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。


## 步骤8 SH1106示例测试

> CLI命令行输入：

```sh
sh1106_init  # 默认初始化sh1106
```

> CLI关键日志：

```sh
sh1106 init test begin ...
open gpio success, fd:513
sh1106 init test end !!!
```

> CLI命令行输入：

```sh
sh1106_display # 在oled信息屏上显示测试
```

> CLI关键日志：

```sh
sh1106 display test begin ...
sh1106 display test end !!!
```
# FAQ

Q1： 本组件只适合sh1106芯片吗？
答：是的，由于不同的硬件环境，不同的OLED驱动，配置的寄存器是不一样的，所以本组件是根据sh1106驱动芯片特性打造出的组件，所以只适合sh1106芯片。如果，其他的oled驱动芯片，建议参考sh1106_init()的实现。

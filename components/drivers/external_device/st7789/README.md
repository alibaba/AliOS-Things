@page st7789 st7789

[更正文档](https://gitee.com/alios-things/st7789/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
LCD (Liquid Crystal Display) 即液晶显示器。AliOS Things 提供了多款LCD组件供开发者使用，适配了目前应用最多的LCD模块。
组件支持以下功能：
- LCD 绘制简单几何图形
- LCD 基于 fbdev组件 (frame buffer) 和 udisplay组件 对接更高层的图形库如 LVGL 或 SDL
- 支持接入多块 LCD

## 版权信息
> Apache license v2.0

## 目录结构
```tree
├── src
│   └── st7789.c                # st7789驱动组件实现
├── include
│   └── st7789.h
├── example
│   └── st7789_example.c        # 测试代码
├── package.yaml                # 编译配置文件
└── README.md                   # 组件使用文档
```

## 依赖组件
* base
* vfs
* spi
* gpio

# 常用配置
``` c
typedef struct _st7789_dev_t
{
    int spi_port;       // 屏幕使用的SPI口
    int spi_freq;       // SPI的驱动频率
    int gpio_dc_id;     // D/C引脚对应开发板的PIN脚
    int gpio_reset_id;  // ResetT引脚对应开发板的PIN脚
    int gpio_bgl_id;    // BackGroundLight引脚对应开发板的PIN脚 对于部分开发板 可以直接接入VCC代表背光常亮

    int spi_fd;
    int gpio_fd;
} st7789_dev_t;
```

# API说明
## st7789硬件初始化
```C
int st7789_hw_init(st7789_dev_t *st7789_dev)
```

|args                                    |description|
|:-----                                  |:----|
|st7789_dev                              |st7789设备结构体 注意以指针形式访问|

## st7789硬件去初始化
```C
int st7789_hw_uninit(st7789_dev_t *st7789_dev)
```

|args                                    |description|
|:-----                                  |:----|
|st7789_dev                              |st7789设备结构体 注意以指针形式访问|

## st7789绘画像素点
```C
void st7789_draw_pixel(st7789_dev_t st7789_dev, uint16_t x, uint16_t y, uint16_t color)
```

|args                                    |description|
|:-----                                  |:----|
|st7789_dev                              |st7789设备结构体|
|x, y                                    |像素点相对屏幕坐标|
|color                                   |画笔颜色 (如白色 0xffff)|


## st7789区域绘画
```C
void st7789_draw_area(st7789_dev_t st7789_dev, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *frame)
```

|args                                    |description|
|:-----                                  |:----|
|st7789_dev                              |st7789设备结构体|
|x, y                                    |区域左上角相对屏幕坐标|
|width, height                           |区域宽高|
|frame                                   |绘画内容|

## st7789绘画矩形
```C
void st7789_draw_rect(st7789_dev_t st7789_dev, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
```

|args                                    |description|
|:-----                                  |:----|
|st7789_dev                              |st7789设备结构体|
|x, y                                    |矩形左上角相对屏幕坐标|
|width, height                           |矩形宽高|
|color                                   |画笔颜色 (如白色 0xffff)|

## st7789全屏填充
```C
void        st7789_draw_frame(st7789_dev_t st7789_dev, uint8_t *frame)
```

|args                                    |description|
|:-----                                  |:----|
|st7789_dev                              |st7789设备结构体|
|frame                                   |待填充数据，构建frame时需根据屏幕分辨率来确定大小|

# 使用示例
组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加st7789组件
> helloworld_demo组件的package.yaml中添加
```yaml
depends:
  - st7789: master # helloworld_demo中引入st7789组件
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install st7789

```

上述命令执行成功后，组件源码则被下载到了./components/drivers/external_device路径中。

## 步骤4 添加示例代码
> 向st7789组件的package.yaml中添加example
```yaml
source_file:
  - "src/*.c"
  - "example/st7789_example.c" # add st7789_example.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 st7789示例测试
> CLI命令行输入：
```sh
example_st7789_hw_init
example_st7789_draw_ract
```
> CLI日志：
```sh
drawing rectangle at 233 55 183 159 in color 1AC1
```

# 注意事项

# FAQ

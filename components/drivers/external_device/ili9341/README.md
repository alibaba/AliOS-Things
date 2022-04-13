@page ili9341 ili9341

# 概述
LCD (Liquid Crystal Display) 即液晶显示器。AliOS Things 提供了多款LCD组件供开发者使用，适配了目前应用最多的LCD模块。
组件支持以下功能：
- LCD 绘制简单几何图形
- LCD 基于 fbdev组件 (frame buffer) 和 udisplay组件 对接更高层的图形库如 LVGL 或 SDL
- 支持接入多块 LCD

## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── src
│   ├── ili9341.c                # ili9341驱动组件实现
│   └── ili9341_fb.c             # ili9341驱动注册到fbdev的函数实现
├── include
│   ├── ili9341.h
│   └── ili9341_fb.h             # ili9341驱动注册到fbdev的函数头文件
├── example
│   └── ili9341_example.c        # 测试代码
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
typedef struct _ili9341_dev_t
{
    int spi_port;       // 屏幕使用的SPI口
    int spi_freq;       // SPI的驱动频率
    int gpio_dc_id;     // D/C引脚对应开发板的PIN脚
    int gpio_reset_id;  // ResetT引脚对应开发板的PIN脚
    int gpio_bgl_id;    // BackGroundLight引脚对应开发板的PIN脚 对于部分开发板 可以直接接入VCC代表背光常亮

    int spi_fd;
    int gpio_fd;
} ili9341_dev_t;
```

# API说明
## ili9341硬件初始化
```C
int ili9341_hw_init(ili9341_dev_t *ili9341_dev)
```

|args                                    |description|
|:-----                                  |:----|
|ili9341_dev                              |ili9341设备结构体 注意以指针形式访问|

## ili9341硬件去初始化
```C
int ili9341_hw_uninit(ili9341_dev_t *ili9341_dev)
```

|args                                    |description|
|:-----                                  |:----|
|ili9341_dev                              |ili9341设备结构体 注意以指针形式访问|

## ili9341绘画像素点
```C
void ili9341_draw_pixel(ili9341_dev_t ili9341_dev, uint16_t x, uint16_t y, uint16_t color)
```

|args                                    |description|
|:-----                                  |:----|
|ili9341_dev                              |ili9341设备结构体|
|x, y                                    |像素点相对屏幕坐标|
|color                                   |画笔颜色 (如白色 0xffff)|


## ili9341区域绘画
```C
void ili9341_draw_area(ili9341_dev_t ili9341_dev, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *frame)
```

|args                                    |description|
|:-----                                  |:----|
|ili9341_dev                              |ili9341设备结构体|
|x, y                                    |区域左上角相对屏幕坐标|
|width, height                           |区域宽高|
|frame                                   |绘画内容|

## ili9341绘画矩形
```C
void ili9341_draw_rect(ili9341_dev_t ili9341_dev, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
```

|args                                    |description|
|:-----                                  |:----|
|ili9341_dev                              |ili9341设备结构体|
|x, y                                    |矩形左上角相对屏幕坐标|
|width, height                           |矩形宽高|
|color                                   |画笔颜色 (如白色 0xffff)|

## ili9341全屏填充
```C
void        ili9341_draw_frame(ili9341_dev_t ili9341_dev, uint8_t *frame)
```

|args                                    |description|
|:-----                                  |:----|
|ili9341_dev                              |ili9341设备结构体|
|frame                                   |待填充数据，构建frame时需根据屏幕分辨率来确定大小|

# 使用示例
示例代码参考example/ili9341_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> 向ili9341组件的package.yaml中添加example
```sh
source_file:
  - "src/*.c"
  - "example/ili9341_example.c" # add ili9341_example.c
```

## 添加ili9341组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - ili9341: master # helloworld_demo中引入ili9341组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make helloworld_demo@haaseduk1 -c config && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档。

## ili9341示例测试
> CLI命令行输入(初始化屏幕)：
```sh
ili9341_hw_init_example
```
> CLI命令行输入(随机在屏幕上画矩形)

```sh
ili9341_draw_ract_example 0x0000
```
注： 0x0000 是 RGB565 的颜色设置

## 关键日志
> CLI日志：
```sh
drawing rectangle at 233 55 183 159 in color 1AC1
```

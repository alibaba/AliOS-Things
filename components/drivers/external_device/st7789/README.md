@page st7789 st7789

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
示例代码参考example/st7789_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> 向st7789组件的package.yaml中添加example
```sh
source_file:
  - "src/*.c"
  - "example/st7789_example.c" # add st7789_example.c
```

## 添加st7789组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - st7789: master # helloworld_demo中引入st7789组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make helloworld_demo@haaseduk1 -c config && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档。

## st7789示例测试
> CLI命令行输入：
```sh
example_st7789_hw_init
example_st7789_draw_ract
```

## 关键日志
> CLI日志：
```sh
drawing rectangle at 233 55 183 159 in color 1AC1
```

# 注意事项

# FAQ

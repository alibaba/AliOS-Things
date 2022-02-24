# framebuf - 帧缓冲区

* 模块功能: 

该模块提供一个可用于创建位图、可发送到显示器的通用帧缓冲区。


## 常量说明

- 色彩格式常量

| 常量 | 常量数值 | 常量说明 |
| --- | --- | --- |
| framebuf.MONO_VLSB | 0 | 单色（1位）颜色模式。此模式定义了一个映射，其中一个字节的位为垂直映射，而0位位于屏幕的最顶部。因此，每个字节占据8个垂直像素。 后续字节在连续的水平位置出现，直至到达最右侧的边缘。更多字节从最左边开始低8个像素显示。|
| framebuf.MONO_HLSB | 3 | 单色（1位）颜色模式。此模式定义了一个映射，其中一个字节的位为水平映射。每个字节占据8个水平像素，0位位于最左边。 后续字节在连续的水平位置出现，直至到达最右侧的边缘。更多字节在下一行低1个像素显示。|
| framebuf.MONO_HMSB | 4 | 单色（1位）颜色模式。此模式定义了一个映射，其中一个字节的位为水平映射。每个字节占据8个水平像素，而7位位于屏幕的最左边。 后续字节在连续的水平位置出现，直至到达最右侧的边缘。更多字节在下一行低1个像素显示。 |
| framebuf.RGB565 | 1 | RGB565彩色（16位，5+6+5）颜色模式 |
| framebuf.GS2_HMSB | 5 | 灰度（2位）颜色模式 |
| framebuf.GS4_HMSB | 2 | 灰度（4位）颜色模式 |
| framebuf.GS8 | 6 | 灰度（8位）颜色模式 |

- 字体库类型常量

| 常量 | 常量数值 | 常量说明 |
| --- | --- | --- |
| framebuf.FONT_ASC8_8 | 808 | 内置ASCII字库 |
| framebuf.FONT_ASC12_8 | 1208 | ASCII字符集，字符大小为12*8 [下载地址](https://hli.aliyuncs.com/o/config/font/ASC12_8) |
| framebuf.FONT_ASC16_8 | 1608 | ASCII字符集，字符大小为16*8 [下载地址](https://hli.aliyuncs.com/o/config/font/ASC16_8) |
| framebuf.FONT_ASC24_12 | 2412 | ASCII字符集，字符大小为24*12 [下载地址](https://hli.aliyuncs.com/o/config/font/ASC24_12) |
| framebuf.FONT_ASC32_16 | 3216 | ASCII字符集，字符大小为32*16 [下载地址](https://hli.aliyuncs.com/o/config/font/ASC32_16) |
| framebuf.FONT_HZK12 | 1212 | HZK汉字库字符集，字符大小为12*12 [下载地址](https://hli.aliyuncs.com/o/config/font/HZK12) |
| framebuf.FONT_HZK16 | 1616 | HZK汉字库字符集，字符大小为16*16 [下载地址](https://hli.aliyuncs.com/o/config/font/HZK16) |
| framebuf.FONT_HZK24 | 2424 | HZK汉字库字符集，字符大小为24*24 [下载地址](https://hli.aliyuncs.com/o/config/font/HZK24) |
| framebuf.FONT_HZK32 | 3232 | HZK汉字库字符集，字符大小为32*32 [下载地址](https://hli.aliyuncs.com/o/config/font/HZK32) |

具体使用方法，请参考**set_font_path**。

### set_font_path - 设置字体库文件路径

* 函数原型：
> framebuf.set_font_path(font, path)

* 函数功能：

设置字体库文件路径。此配置作用于全局。

使用 FrameBuffer.text 函数绘制文本之前，需要调用此函数指定字体库路径。当字体库路径被指定后，后续创建的 FrameBuffer 类都将使用此配置。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|font|int|是|字体类型，参见**字体库类型常量**|
|path|string|是|字体库在文件系统中的路径，可从**字体库类型常量**表中下载后，放置于项目文件中推送至开发板|

* 使用示例：
```python
import framebuf

# 字库文件存放于项目目录 font, 注意若用到了中英文字库则都需要放置
framebuf.set_font_path(framebuf.FONT_HZK12, '/data/pyamp/font/HZK12')
framebuf.set_font_path(framebuf.FONT_ASC12_8, '/data/pyamp/font/ASC12_8')
# 创建 FrameBuffer 对象
fbuf = framebuf.FrameBuffer(bytearray(10 * 100 * 2), 10, 100, framebuf.RGB565)
# 在 FrameBuffer 上绘制文本
fbuf.text("欢迎使用HaaS",0,28,1, size = 12, zoom = 2, space = 4)
```

## calss FrameBuffer - 帧缓冲区类

* 类说明：
帧缓冲区类提供一个像素缓冲区，此缓冲区可使用像素、线、矩形、文本甚至其他帧缓冲区来绘制。此缓冲区可在生成显示器输出时发挥作用。

### 构造函数

* 函数原型：
> framebuf.FrameBuffer(buffer, width, height, format, stride=width)

* 函数功能：
构造一个 FrameBuffer 对象。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|buffer|bytearray|是|buffer 缓冲区是一个带有缓冲区协议的对象，且其大小须足以容纳由宽度、高度和帧缓冲区定义的每个像素。|
|width|int|是|width 宽度为**以像素为单位**的帧缓冲区的宽度。|
|height|int|是| height 高度为**以像素为单位**的帧缓冲区的高度。|
|format|int|是| format 形式指定用于帧缓冲区的像素类型；可选参数请见**常量说明** |
|stride|int|否| stride 跨度是帧缓冲区中每条水平线之间的像素数量。此参数默认值为宽度， 但是在另一更大帧缓冲区或屏幕中实现一个帧缓冲区时可能需要调整。缓冲区空间须适应增大的步骤大小。 |

> 必须指定有效缓冲区、宽度、高度和可选跨度。无效帧缓冲区大小或维度可能会导致意外错误。

* 示例：
```python
import framebuf

# 对于每个RGB565像素，帧缓冲区都需2字节
fbuf = framebuf.FrameBuffer(bytearray(10 * 100 * 2), 10, 100, framebuf.RGB565)
```

### pixel - 读取或设置某个像素

* 函数原型：
> FrameBuffer.pixel(x, y[, c])

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|x|int|是| 像素横坐标 |
|y|int|是| 像素纵坐标 |
|c|int|否| 色值。若给定c，将指定像素设置到给定颜色。 |

* 返回值
若未给定c，则获取指定像素的色值。

### fill - 使用指定颜色填满整个帧缓冲区。

* 函数原型：
> FrameBuffer.fill(c)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|c|int|是| 填充颜色，需要和FrameBuffer的色彩格式对应。 |

### hline - 绘制水平线
### vline - 绘制垂直线

* 函数原型：
> FrameBuffer.hline(x, y, w, c)
> FrameBuffer.vline(x, y, w, c)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|x|int|是| 起点像素横坐标 |
|y|int|是| 起点像素纵坐标 |
|w|int|是| 线宽 |
|c|int|否| 色值 |

### line - 绘制任意直线

* 函数原型：
> FrameBuffer.line(x1, y1, x2, y2, c)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|x1|int|是| 起点像素横坐标 |
|y1|int|是| 起点像素纵坐标 |
|x2|int|是| 纵点像素横坐标 |
|y2|int|是| 终点像素纵坐标 |
|c|int|是| 色值 |

### rect - 绘制空矩形
### fill_rect - 绘制填充矩形

* 函数原型：
> FrameBuffer.rect(x, y, w, h, c)
> FrameBuffer.fill_rect(x, y, w, h, c)

* 函数说明：

在给定位置、按照给定大小和颜色绘制一个矩形。 rect 类函数仅绘制1个像素的边框，而 fill_rect 类函数可绘制出边框和矩形内部。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|x|int|是| 起点像素横坐标，以矩形左上角为起点 |
|y|int|是| 起点像素纵坐标，以矩形左上角为起点|
|w|int|是| 矩形宽 |
|w|int|是| 矩形高 |
|c|int|否| 色值 |

### scroll

* 函数原型：
> FrameBuffer.scroll(xstep, ystep)

* 函数功能：
使用给定向量转换帧缓冲区的内容。这可能会在帧缓冲区中留下之前颜色的覆盖区。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|xstep|int|是| 横向滚动距离 |
|ystep|int|是| 纵向滚动距离 |

### blit

* 函数原型：
> FrameBuffer.blit(fbuf, x, y[, key])

* 函数功能：

在当前帧缓冲区的顶部的给定坐标下绘制另外一个帧缓冲区。若指定*key*，则其应为一个颜色整数，且相应颜色被认为是透明的：所有具有该色值的像素都不会被绘制。

此类函数在帧缓冲区使用的不同格式间运行，但是由于颜色格式不匹配，最终颜色可能与预想的存在差异。

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|fbuf|FramebBuffer|是| 帧缓冲区对象 |
|x|int|是| 帧缓冲区起点横坐标 |
|y|int|是| 帧缓冲区起点纵坐标 |
|key|int|否| 设置透明色 |

### text

* 函数原型：
> FrameBuffer.text(s, x, y, c[, size, zoom, space])

* 函数功能：

在FrameBuffer上绘制文本

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|s|string|是| 待显示的字符串。若字符串中包含中文字符，请使用GBK编码。 |
|x|int|是| 文本左上角横坐标 |
|y|int|是| 文本左上角纵坐标 |
|c|int|是| 色值 |
|size|int|否| 字体大小，支持12，16，24，32。默认为12 |
|zoom|int|否| 字符缩放倍数。默认为1 |
|space|int|否| 字符间距。默认为0 |

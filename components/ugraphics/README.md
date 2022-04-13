@page ugraphics ugraphics

# 概述
ugraphics组件是基于SDL2封装的接口，支持JPEG/PNG图像解码绘制、点/线/框图像等绘制、图像旋转、RGB格式转换、中英文字符绘制等功能，方便用户进行基础的图像应用开发。

# 版权信息
> Apache 2.0 License

# 目录结构
```sh
.
├── include
│   ├── ugraphics.h                 # 对外头文件
│   ├── format
│   │    └── to_rgb565.h            # rgb格式转换头文件
│   └── rotate
│        └── rotate.h               # rotate旋转头文件
├── package.yaml                    # 编译配置文件
└── src
    ├── ugraphics.c                 # 图像操作函数代码
    ├── example
    │    └── ugraphics_example.c    # 图像旋转头文件
    ├── format
    │    └── to_rgb565.c            # rgb格式转换函数代码
    ├── jpegdec
    │    └── jpegdec.c              # 基于libjpeg实现的图像解码实现
    └── rotate
         └── rotate.c               # 图像旋转函数代码
```

# 依赖组件

* SDL2
* fatfs
* littlefs


# 常用配置
```sh
def_config:                              # 组件的可配置项
  AOS_COMP_JPEG: 1
  CONFIG_UGRAPHICS_FORMAT: 1
  CONFIG_UGRAPHICS_ROTATE: 1
```
> 配置是否支持JPEG解码、图像格式转换、图像旋转，默认是关闭的

# API说明
@ref ugraphics_aos_api

# 使用示例
示例代码参考example/ugraphics_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> ugraphics组件的package.yaml中添加example
```sh
source_file:
  - "example/ugraphics_example.c" # add ugraphics_example.c
```

## 添加ugraphics组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - ugraphics: dev_aos            # helloworld_demo组件中引入ugraphics组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档。

## ugraphics示例测试

### CLI命令行输入：
```sh
ugraphics draw rect # 绘制矩形框
```

> CLI关键日志：
```sh
ugraphics draw rectangle ok!
```

### CLI命令行输入：
```sh
ugraphics draw jpg # 绘制图片
```

> CLI关键日志：
```sh
ugraphics draw jpg image ok!
```

### CLI命令行输入：
```sh
ugraphics draw png # 绘制图片
```

> CLI关键日志：
```sh
ugraphics draw png image ok!
```

### CLI命令行输入：
```sh
ugraphics draw line # 绘制线条
```

> CLI关键日志：
```sh
ugraphics draw line ok!
```

### CLI命令行输入：
```sh
ugraphics draw string # 绘制字符串
```

> CLI关键日志：
```sh
ugraphics draw string ok!
```

### CLI命令行输入：
```sh
ugraphics fill rect # 绘制字符串
```

> CLI关键日志：
```sh
ugraphics fill rectangle ok!
```

### CLI命令行输入：
```sh
ugraphics clear # 绘制字符串
```

> CLI关键日志：
```sh
ugraphics clear screen ok!
```

# 注意事项
在使用绘制字符串或图片时，因为需要加载字体或图片，字库simfang.ttf或图片默认存放在/data/font/目录，因此需要在depends中确保littlefs组件已经加入。烧录程序时除烧录demo image外，还需要通过以下命令烧录文件系统。
```sh
aos burn -f hardware/chip/haas1000/release/write_flash_tool/ota_bin/littlefs.bin#0xB32000
```
# FAQ
NA



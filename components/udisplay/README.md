@page udisplay udisplay

# 概述

udisplay是一套图形接口组件，AliOS Things图形组件包含该部分，用于连接用户层和驱动层的部分。
用户通过调用udisplay函数来进行图形处理，udisplay通过posix接口调用fbdev设备驱动，避免用户和驱动直接交互。

该组件支持以下功能：
- udisplay init
- udisplay draw

# 版权信息
> Apache 2.0 License

# 目录结构
```sh
.
├── include
│   ├── udisplay.h            # 对外头文件
│   └── udispaly_cli.h        # 对外头文件
├── package.yaml              # 编译配置文件
└── src
│   └── udisplay.c            # udisplay核心处理层
└──example
    └── udisplay_example.c    # udisplay测试用例代码
```

# 依赖组件

* osal_aos
* drivers

# 常用配置

# API说明
@ref udisplay_aos_api

@ref udisplay_cli_aos_api


# 使用示例
示例代码参考example/udisplay_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> udispaly组件的package.yaml中添加
```C
source_file:
  - "src/*.c"
  - "example/udisplay_example.c" # add udisplay_example.c
```

## app中添加udisplay组件
> helloworld_demo组件的package.yaml中添加
```C
depends:
  - udispaly: dev_aos # helloworld_demo 中引入 udisplay 组件
  - ili9341: master   # helloworld_demo 中引入 ili9341 屏幕驱动
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档。

## udisplay示例测试
> udisplay 初始化命令：
```sh
udisplay_init_example
```
> CLI命令行输入：
```sh
udisplay_example pattern 16 0x0000
```

+ pattern 是测试pattern固定字符.
+ 16: ili9341 屏幕为 RGB565
+ 0x0000  RGB565 颜色设置

# FAQ

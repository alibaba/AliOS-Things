@page py_engine py_engine

# 概述
py_engine (Python轻应用引擎) 以MicroPython为基础打造而成.可以快速实现IoT场景连云、控端、AI等最常见功能。快速上手文档和编程接口请参考[Python轻应用编程参考](https://g.alicdn.com/HaaSAI/PythonDoc/quickstart/index.html)
组件支持以下功能：
- 连云：支持基于linkSDK连接阿里云物联网平台
- 控端：支持PWD、UART、SPI、I2C、ADC、DAC、GPIO等基本硬件接口控制
- AI：支持端上AI和云端AI能力，覆盖人脸、人体、视频、文字等150+场景

## 版权信息
> Apache license v2.0

## 目录结构
```sh
py_engine
├── call_cmake.sh                    # 系统引导编译cmake的脚本文件
├── CMakeLists.txt                   # cmake编译脚本文件
├── example
│   └── py_engine_example.c          # 示例代码
├── micropython                      #Python解释器以及面向AliOS Things的适配
├── minicv                           #精简版OPENCV，包含摄像头数据采集、AI等
├── package.yaml                     # 编译配置文件
├── platform                         #OS隔离层
└── README.md                        #帮助文档
    
    ```

## 依赖组件
* rhino
* cli
* haas100
* osal_aos
* ulog
* kv
* mbedtls
* cjson
* fatfs
* haas1000
* vfs

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> AOS_COMP_VFS: 默认1, 如需修改，在yaml中修改AOS_COMP_VFS配置
```sh
def_config:
  AOS_COMP_VFS: 1
  ```
  > fatfs，默认1，可按照实际需要修改yaml配置如：
  ```sh
  def_config:
    AOS_COMP_FATFS: 2
    ```

# API说明
@ref mpy_run
@ref mpy_init
@ref mpy_add_path

# 使用示例
示例代码参考example/py_engine_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> py_engine组件的CMakeLists.txt 文件 file(GLOB SOURCEFILE "") 语句后面添加example配置:
```sh
include(${EXAMPLEDIR}/aos.mk)
    ```

## 添加py_engine组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - py_engine: dev_aos # helloworld_demo中引入py_engine组件
  ```

## 编译
```sh
cd solutions/helloworld_demo && aos make clean && aos make V=1
```
```sh
aos --version
```

## 烧录固件
> 参考具体板子的快速开始文档。

## pyEngine示例测试
> CLI命令行输入：
```sh
python
```

## 关键日志
> CLI日志：
```sh
start micropython!
```

# 注意事项
如果需要使用py_engine，就需要对接两个接口：**mpy_init** and **mpy_run**。(declare in `mpy_main.h`)
```

# FAQ
Q1： python 使用示例？
> from driver import ADC
> adc = ADC()
> adc.open("ADC0")
> value = adc.read()
> adc.close()

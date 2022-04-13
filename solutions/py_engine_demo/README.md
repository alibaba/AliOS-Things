@page py_engine_demo py_engine_demo
# 1. 案例简介

py_engine (Python轻应用引擎) 以MicroPython为基础打造而成, 基于py_engine可以快速实现IoT场景连云、控端、AI等最常见功能。快速上手文档和编程接口请参考[Python轻应用快速上手](https://g.alicdn.com/HaaSAI/PythonDoc/quickstart/index.html)
* 连云：支持基于linkit连接阿里云物联网平台
* 控端：支持PWD、UART、SPI、I2C、ADC、DAC、GPIO等基本硬件接口控制
* AI：支持端上AI和云端AI能力，覆盖人脸、人体、视频、文字等150+场景

py_engine 默认作为一个组件存在于alios things 中。

py_engine_demo 依赖了py_engine 组件，主要功能如下:

* 注册python命令
* 启动python虚拟机

py_engine_demo 只有一个appdemo.c,注册了python cli 命令后就退出了，等待用户指令启动python虚拟机

# 2. 基础知识
## 2.1 组件依赖方法
* 在package.yaml 中增加py_engine组件依赖
```
    depends:
        - py_engine: dev_aos
```
* 增加头文件
```
    #include "mpy_main.h"
```

* 调用初始化方法
```
    mpy_init();
    mpy_run(argc, argv);
```



# 3. 物料清单

## 3.1 HaaS100 硬件

[HaaS100 硬件简介](https://help.aliyun.com/document_detail/184426.html)

<img src="https://img.alicdn.com/imgextra/i4/O1CN01XxD6Xo217CB3FZnEU_!!6000000006937-2-tps-746-497.png" style="zoom:80%;" />

# 4. 案例实现

## 4.1 硬件连接
该案例只需要连接电源线以及串口线，如下图所示：
![硬件连接.png](https://img.alicdn.com/imgextra/i2/O1CN01S9jkJw1dihpqURQH4_!!6000000003770-0-tps-1280-960.jpg)

## 4.2 软件实现

### 4.2.1 编译固件
```sh
# cd ../../solutions/py_engine_demo
# aos make
```

### 4.2.2 验证Python功能

烧录固件，连接窗口，进入shell，通过python命令进入python rpel模式
```sh
# python
# python on HaaS100 by 2021-03-17， press ctrl+d to exit！
>>> print("hello-world")
hello-world
```

# 5. 总结

该demo只依赖AliOS Things，不依赖具体的硬件,通过这个demo ，默认可以将py_engine 按照两种不同的方式运行。



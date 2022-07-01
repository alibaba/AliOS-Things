
HaaS Python 工程
=======================

HaaS Python是阿里云IoT团队最新研发的一套低代码编程框架，兼容MicroPython编程规范，依托HaaS平台软硬件积木提供AI、支付、蓝牙配网、云连接、UI等物联网场景常用的能力，从而解决了物联网应用开发难的问题。有了Python轻应用框架，物联网编程不再局限于专业软件开发人员，一般的技术员也可以快速实现复杂的物联网需求。
更多HaaS Python介绍和开发资料见[HaaS Python官网](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1dZDX71#/)，[创意案例](https://haas.iot.aliyun.com/solution)，[硬件积木](https://haas.iot.aliyun.com/solution/hardware)

HaaS Python is a set of low-code programming frameworks newly developed by Alibaba Cloud IoT team. It is compatible with MicroPython programming specifications. It relies on the hardware and software building blocks of the HaaS platform to provide capabilities commonly used in IoT scenarios such as AI, payment, Bluetooth network configuration, cloud connection, and UI. Solve the difficult problem of IoT application development. With the Python light application framework, IoT programming is no longer limited to professional software developers, and general technicians can quickly implement complex IoT requirements.
For more HaaS Python introduction and development materials, see [HaaS Python official website](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1dZDX71#/), [Creative Case](https://haas.iot.aliyun.com/solution), [Hardware Building Blocks](https://haas.iot.aliyun.com/solution/hardware)

该仓库包含的主要组件为：

- py/ -- 核心 Python 实现，包括编译器、运行时和核心库。
- mpy-cross/ -- 用于将转换脚本预编译为字节码的交叉编译器。
- external/ -- 扩展三方C组件。
- modules/ -- 扩展 Python 接口。
- tests/ -- 测试框架和测试脚本。
- docs/ -- HaaS Python官方文档及相关案例。

附加部分:
- ports/esp32/ -- 运行在Espressif的ESP32 SoC上的HaaS Python版本。
- ports/stm32/ -- 在PyBoard和类似的STM32板上运行的HaaS Python版本（使用ST的Cube HAL驱动程序）。
- ports/rp2/ -- 运行在Raspberry-Pi-Pico SoC上的HaaS Python版本。
- ports/haas/ -- 运行在 [AliOS-Things](https://github.com/alibaba/AliOS-Things) 系统， HaaS1000 SoC上的HaaS Python版本。
- extmod/ -- 用C实现的附加（非核心）模块。
- tools/ -- 各种工具，包括 pyboard.py 模块。
- examples/ -- 一些示例 Python 脚本。

HaaS Python 交叉编译器 mpy-cross
-----------------------------------------

大多数端口都需要先构建 HaaS Python 交叉编译器。这个名为 mpy-cross 的程序用于将 Python 脚本预编译为 .mpy 文件，然后可以将这些文件包含（冻结）到固件/可执行文件的端口中。


构建 mpy-cross 使用:

    $ cd mpy-cross
    $ make

The STM32 version
-----------------

“stm32” 端口需要 ARM 编译器 arm-none-eabi-gcc 和相关的 bin-utils。
使用下面的命令编译：

    $ cd ports/stm32
    $ make

Contributing
------------

HaaS Python 是一个开源项目，欢迎贡献。
HaaS Python 在 MIT 许可下获得许可，所有贡献都应遵循此许可。

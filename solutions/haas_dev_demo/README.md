@page haas_dev_demo JavaScript和Python双引擎运行环境

[更正文档](https://gitee.com/alios-things/haas_dev_demo/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 1. 案例简介
## 1.1 Python轻应用引擎
py_engine (Python轻应用引擎) 以MicroPython为基础打造而成, 基于py_engine可以快速实现IoT场景连云、控端、AI等最常见功能。快速上手文档和编程接口请参考[Python轻应用快速上手](https://g.alicdn.com/HaaSAI/PythonDoc/quickstart/index.html)
* 连云：支持基于linkit连接阿里云物联网平台
* 控端：支持PWD、UART、SPI、I2C、ADC、DAC、GPIO等基本硬件接口控制
* AI：支持端上AI和云端AI能力，覆盖人脸、人体、视频、文字等150+场景

py_engine 默认作为一个组件存在于alios things 中。

py_engine_demo 依赖了py_engine 组件，主要功能如下:

* 注册python命令
* 启动python虚拟机

py_engine_demo 只有一个appdemo.c,注册了python cli 命令后就退出了，等待用户指令启动python虚拟机

## 1.2 JavaScript轻应用
JavaScript轻应用引擎是一套运行在资源受限设备上的轻量级应用软件框架。

业务应用代码脚本化，使用JavaScript脚本迅速构建轻应用。
嵌入式系统底座和应用完全解耦，使得IoT设备端应用生态的广度大幅拓宽。

Runtime采用JavaScript引擎加载用户脚本代码并执行，配合高可用的服务和工具提供应用的在线分发、更新及运维
* Runtime采用JavaScript应用引擎加载用户脚本代码并执行，配合高可用的服务和工具提供应用的在线分发、更新及运维
* 无需编译、烧录即可开发在物联网设备上运行的轻量级应用，快速开发，高效运维
* 可通过云存储托管设备端应用和资源，也通过集成云服务API，使得云端计算能力获得在设备端的延伸

# 2. 基础知识
## 2.1 基础目录结构

```tree
.
├── haas_dev_demo.c   # 该solution核心打印输出代码，入口**application_start**
├── k_app_config.h # 内核组件的配置开关，优先级低于**k_config.h**
├── maintask.c     # 系统主任务入口处理，入口**aos_maintask**
├── Makefile       # aos make编译时入口
├── package.yaml   # 编译系统配置文件
└── SConstruct     # Makefile => Scon => aostools
```

## 2.2 组件依赖方法
* 在package.yaml 中增加py_engine及amp组件依赖
py_engine是Python轻应用引擎的组件，amp是JavaScript轻应用引起的组件。
```yaml
    depends:
        - py_engine: master
        - amp: master
```

# 3. 物料清单

## 3.1 HaaS100 硬件

[HaaS100 硬件简介](https://help.aliyun.com/document_detail/184426.html)

<div align=left display=flex>
    <img src="https://img.alicdn.com/imgextra/i4/O1CN01XxD6Xo217CB3FZnEU_!!6000000006937-2-tps-746-497.png" style="max-width:800px;" />
</div>

# 4. 案例实现

## 4.1 硬件连接
该案例只需要连接电源线以及串口线，如下图所示：

<img src="https://img.alicdn.com/imgextra/i2/O1CN01S9jkJw1dihpqURQH4_!!6000000003770-0-tps-1280-960.jpg" style="max-width:800px;" />

## 4.2 软件实现



* AliOS Things开发环境搭建

    开发环境的搭建请参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，其中详细的介绍了AliOS Things 3.3的IDE集成开发环境的搭建流程。


* 代码下载
    py_engine代码下载请参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)，其中，
    > 选择解决方案: "haas开发框架JS引擎及Python引擎简单示例"
    > 选择开发板: HaaS200


*  编译

    参考 [《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。


* 烧录

    由于该demo的资源文件位于/data，分区，因此烧录的时候需要烧录littlefs.bin,方法如下：

    haas100将hardware/chip/haas1000/package.yaml文件中以下代码段的注释打开:
```yaml
  program_data_files:
    - filename: release/write_flash_tool/ota_bin/littlefs.bin
      address: 0xB32000
```

    haas200将hardware/chip/rtl872xd/package.yaml文件中以下代码段的注释打开:
```yaml
  program_data_files:
    - filename: prebuild/littlefs.bin
      address: 0x08314000
```

    参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)。

## 4.3 验证Python功能

连接串口，通过python命令进入python repl模式
```sh
# python
# python on HaaS100 by 2021-03-17， press ctrl+d to exit！
>>> print("hello-world")
hello-world
```

## 4.3 验证JavaScript功能

连接串口，通过amp_example命令进入JavaScript模式
```sh
amp_example
```
执行完后，HaaS200板子上灯会开始闪烁，串口打印出led设置的信息。

# 5. 总结

该demo只依赖AliOS Things，不依赖具体的硬件,通过这个demo ，默认可以将py_engine 按照两种不同的方式运行。

* # B-U585I-IOT02A 开发板介绍

  项目涉及的`AliOS Things`操作系统与 `B-U585I-IOT02A` 探索套件的适配工程首先基于 Arm 虚拟硬件(AVH) 的虚拟`B-U585I-IOT02A` 开发板进行了开发和验证, 并在项目整体开发完成后, 将构建好的应用镜像文件直接烧录在物理`B-U585I-IOT02A`开发板上进行了验证。

  ## 一、概述

  [B-U585I-IOT02A](https://www.st.com/zh/evaluation-tools/b-u585i-iot02a.html) 探索套件为STM32U585AI微控制器提供了一个完整的演示和开发平台, 配备 Arm Cortex-M33内核、Arm TrustZone、Armv8-M 主线安全扩展、2MB Flash存储、786KB SRAM存储, 以及智能外设资源。

  ### 1. 主要特点

  * 该探索套件利用低功耗通信、多路传感和直接连接的方式接入到云服务器,支持广泛的物联网应用。
  * 包含 Wi-Fi 和 Bluetooth 模块,以及麦克风、温度和湿度、磁力计、加速度计和陀螺仪、压力、飞行时间、手势检测传感器。
  * 可支持 ARDUINO Uno V3、STMod+和Pmod连接, 可借助专用附加板，实现扩展功能。
  * 板载 STLINK-V3E 调试器提供开箱即用的加载和调试功能, 以及USB虚拟COM端口桥。

  ### 2. 硬件资源(部分)

  * 符合802.11 b/g/n规范的Wi-Fi模块(来自MXCHIP)
  * MEMS传感器(来自意法半导体)
    - 2个数字麦克风
    - 相对湿度和温度传感器
    - 3轴磁力计
    - 3D加速度计和3D陀螺仪
    - 压力传感器，260-1260 hPa绝对数字输出气压计, ...
  * 环境光传感器
  * 2个用户LED
  * 用户按钮
  * 复位按钮
  * 板连接器
    - USB Type-C, ... 

  更多套件功能介绍,请访问ST官网 [B-U585I-IOT02A](https://www.st.com/zh/evaluation-tools/b-u585i-iot02a.html) 产品概述界面。

  ## 二、Arm 虚拟硬件(AVH)

  [Arm 虚拟硬件 (AVH)](https://www.arm.com/products/development-tools/simulation/virtual-hardware) 是 Arm 建模技术的演进，为应用程序开发人员和SoC设计人员提供基于 Arm 架构的处理器、系统和第三方硬件的模型, 以便在芯片完备前就可着手进行软件的开发与测试。它作为一个简单的云应用程序运行,可用于模拟内存和外围设备,为物联网及嵌入式平台引入现代化的敏捷软件开发实践方法,例如:持续集成和持续部署 CI/CD(DevOps) 以及 MLOps 工作流。

  目前, AVH 提供2种方式供开发者立即体验: 

  * AVH Corstone 和 AVH CPU 虚拟硬件模型可通过订阅 [AWS Marketplace](https://aws.amazon.com/marketplace/pp/prodview-urbpq7yo5va7g) 的 `Arm Virtual Hardware` 亚马逊机器镜像 (AMI) 服务直接取用;
  * AVH 第三方硬件可访问 [https://avh.arm.com](https://avh.arm.com/) 注册账号并使用 (由于目前该平台仍处于内测阶段,请访问[页面](https://www.arm.com/resources/contact-us/virtual-hardware-boards)注册获取体验账号。建议使用公司或学校邮箱注册,可加快审批流程及获得更及时的技术支持)

  AVH 第三方硬件模拟了完整的 SoC 系统和开发套件的功能行为,以实现从虚拟硬件到物理硬件的无缝软件移植。目前 AVH 第三方硬件模拟了包括 CPU、传感器和连接模块在内的大部分通用的组件和外围设备,并不断致力于支持更多的第三方硬件 IP、NPU、ISP 和 VSP 等。目前, 可体验的第三方 IoT 虚拟硬件开发板主要包括:树莓派的 `Raspberry Pi 4`; 意法半导体的 `STM32U5 IoT Discovery Kit (即 B-U585I-IOT02A)` 及 恩智浦半导体的`i.MX 8m`。

  本项目涉及的`AliOS Things`操作系统与 `B-U585I-IOT02A` 探索套件的适配工程正是首先基于 Arm 虚拟硬件(AVH) 的虚拟 `B-U585I-IOT02A` 开发板进行了开发和验证, 并在项目整体开发完成后, 将最终构建好的应用镜像文件直接烧录在物理`B-U585I-IOT02A`开发板上进行了再次验证。


  ## 三、解决方案示例代码

  * hello_world_demo 示例工程 —— [快速开始](https://github.com/852432070/avh-alios/tree/main/helloworld_demo)
  * micro_speech_demo 示例工程 —— [快速开始](https://github.com/852432070/avh-alios/tree/main/tflite_micro_speech_demo)
  * mqtt_demo 示例工程 —— [快速开始](https://github.com/852432070/avh-alios/tree/main/mqtt_demo)


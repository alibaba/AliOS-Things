[EN](./README.md) | 中文

## 介绍

JSEngine 提供了在设备端用 Javascript 语言开发产品的应用开发框架，运行环境，以及开发和部署工具集，旨在让 IoT 设备应用和开发者不用过度关注硬件平台的差异性，底层驱动和系统的实现细节，也不用关注具体云端设备接入技术，就可以快速开发出IoT产品，并能实现最终应用跨硬件平台的运行能力，真正做到“ 一份代码，到处运行 ”。

### 特性

JSEngine 具备如下特点：

* 提供针对IoT应用特点而优化的高性能 JS 引擎：
    * 资源占用少：RAM< 15KB,ROM(Footprint) < 15KB；
    * CPU性能优化：优化的词法和句法分析器，精简的Javascript的语法支持；
    * 完备的JS语法支持能力：IoT应用的精简语法，CommonJS扩展语法，大量IoT Builtin模块；
* 包含IoT设备端的应用编程框架和设备模型：
    * 设备模型兼容阿里的标准设备模型（属性，事件，方法）；
    * 自带设备上云能力模块，与阿里的Link Platform IoT平台对接；
    * 支持板级驱动，模块及设备驱动的动态加载能力，应用可本地和云端加载；
    * 支持多芯片平台移植(HAL)，已涵盖了庆科，乐鑫，STM，MTK等主流芯片；
* 提供方便的本地和云端开发工具：
    * WebIDE 支持本地和云端应用代码编辑，调试和发布；
    * `be-cli` 命令行工具可让开发者在本地开发，调试设备端应用；
* 提供丰富的设备，驱动，设备应用开发的工具，模块：
    * 实现并开源大量的通用设备驱动模块，场景应用，供开发者引用；
    * 提供应用，设备模块下载和分发通道和能力；
    * 开发者可提交模块供第三方应用；

JSEngine 适用于IoT设备端的场景应用开发，设备开发，驱动模块等开发者。

## 目录

```
.
├── docs                    # 文档
├── scripts                 # js脚本，包括应用示例、驱动、模块等。
└── src
    ├── components          # 组件，比如miio
    ├── engine              # JSEngine包括2个js虚拟机，DuktapeJSE和LiteJSE（自研）
    │   ├── duktape_engine
    │   └── lite_engine
    ├── include             # 头文件
    ├── main                # JSEngine 入口
    ├── port                # JSEngine 移植接口
    ├── services            # JSEngine 服务，比如app manager, board manager, debugger等
    └── utils
```

##编译固件及下载

### Building and Flashing

**编译固件：**

```sh
# generate jsengine_app@yourboard default config
aos make jsengine_app@yourboard -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

> 目前已经验证过的开发板有：`mk3060`, `esp32devkitc`, and `developerkit`。

**烧录固件：**

```sh
aos upload
```

## JS程序调试方法

### 方式一：通过be-cli工具进行调试。

#### Step1: 工具安装

在framework/JSEngine/tools目录执行```make cli```安装并更新be_cli工具。

说明：be_cli工具也可以手动安装（需指明版本号）：npm install be-cli@0.1.32 -g

* 参考docs/开发工具使用指南目录下的《be-cli 工具使用指南.md》，并通过be_cli 更新设备js程序。

#### Step2: JS程序更新到设备

* 在samples目录下有多个module和app的js例子程序，供参考试用。
* 例如我们需要调试gw的例子，使用如下几个步骤：
  * 使用```be -p samples/app/gw``` 将gw目录中的js文件打包成一个app.bin。
  * 使用```be connnect``` 连接模组/开发板的usb串口。
  * 使用```be push app.bin```  将app.bin更新到设备上。
  * 重启设备，设备启动后会自动加载并运行index.js这个文件。

**说明**：如果在使用be-cli工具过程中遇到问题，也可以采用方式二进行js文件到打包下载。



### 方式二：通过make spifffs命令打包js文件并download

* 进入```framework/JSEngine/tools```目录

* 将您的js程序拷贝到```framework/JSEngine/tools/jspack```目录，必须包含一个名为index.js的程序。

  说明：jspack目录默认有一个index.js程序，里面实现了通过js console.log的helloworld打印。

* 根据您当前使用的硬件版型修改：```framework/JSEngine/tools/jspack/Makefile``` 中的PRODUCT_TYPE定义：默认情况下 为developerkit。

* 在```framework/JSEngine/tools```目录执行```make spiffs```; 执行成功会生成一个spiffs.bin文件。

* 最后执行```make download_js```命令 下载spiffs.bin文件到设备。

* 最后断电重启设备。

## 参考文档







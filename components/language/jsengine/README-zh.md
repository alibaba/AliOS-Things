[EN](./README.md) | 中文

## 介绍

JSEngine 提供了在设备端用 Javascript 语言开发产品的应用开发框架，运行环境，以及开发和部署工具集，旨在让 IoT 设备应用和开发者不用过度关注硬件平台的差异性，底层驱动和系统的实现细节，也不用关注具体云端设备接入技术，就可以快速开发出IoT产品，并能实现最终应用跨硬件平台的运行能力，真正做到“ 一份代码，到处运行 ”。

### 特性

JSEngine 具备如下特点：

* 提供针对IoT应用特点而优化的高性能 JS 引擎：
    * 资源占用少：RAM< 15KB,ROM < 45KB；
    * CPU性能优化：优化的词法和句法分析器，精简的Javascript的语法支持；
    * 完备的JS语法支持能力：IoT应用的精简语法，CommonJS扩展语法，大量IoT Builtin模块；
* 包含IoT设备端的应用编程框架和设备模型：
    * 设备模型兼容阿里的标准设备模型（属性，事件，方法）；
    * 自带设备上云能力模块，与阿里的Link Platform IoT平台对接；
    * 支持板级驱动，模块及设备驱动的动态加载能力，应用可本地和云端加载；
    * 支持多芯片平台移植(HAL)，已涵盖了庆科，乐鑫，STM，MTK等主流芯片；
* 提供方便的本地和云端开发工具：
    * [be-cli](https://www.npmjs.com/package/be-cli) 命令行工具可让开发者在本地开发，调试设备端应用；
* 提供丰富的设备，驱动，设备应用开发的工具，模块：
    * 实现并开源大量的通用设备驱动模块，场景应用，供开发者引用；
    * 提供应用，设备模块下载和分发通道和能力；
    * 开发者可提交模块供第三方应用；

JSEngine 适用于IoT设备端的场景应用开发，设备开发，驱动模块等开发者。

### 架构

![](https://img.alicdn.com/tfs/TB18W2gkoT1gK0jSZFhXXaAtVXa-2387-1558.png)

### 简单示例

![](https://yuque.antfin-inc.com/api/filetransfer/images?url=https%3A%2F%2Fimg.alicdn.com%2Ftfs%2FTB1vO3JiET1gK0jSZFhXXaAtVXa-1024-720.gif&sign=99c20b6f96feb960affca12d0dd547e663ba79805bde75b996783a6580817fbd)

## 目录

```
.
├── docs                    # 文档
├── scripts                 # js脚本，包括应用示例、驱动、模块等。
└── src
    ├── components          # 组件
    ├── engine              # JSEngine包括2个js虚拟机，DuktapeJSE和LiteJSE（自研）
    │   ├── duktape_engine
    │   └── lite_engine
    ├── include             # 头文件
    ├── main                # JSEngine 入口
    ├── port                # JSEngine 移植接口
    ├── services            # JSEngine 服务，比如app manager, board manager, debugger等
    └── utils
```

## 编译及下载固件

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

> 目前已经验证过的开发板有：`mk3060`, `esp32devkitc`, `stm32f429zi-nucleo` 以及 `developerkit`。

**烧录固件：**

```sh
aos upload
```
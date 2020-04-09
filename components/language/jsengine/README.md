EN | [中文](./README-zh.md)

## Contents

## Introduction

JSEngine provides an application development framework, runtime environment, and development and deployment toolset for developing products in the JS language on the device side. It is designed to allow IoT device applications and developers to not pay too much attention to hardware platform differences, the underlying drivers and system implementation. In detail, you don't need to pay attention to the specific cloud device access technology, you can quickly develop IoT products, and can achieve the ultimate application cross-hardware platform running ability, truly "a code, run everywhere."

### Features

JSEngine has the following features:

* Provides a high-performance JS engine optimized for IoT application features:
    * Less resource usage: RAM< 15KB,ROM < 45KB;
    * CPU performance optimization: optimized lexical and syntactic parser, streamlined Javascript syntax support;
    * Complete JS syntax support: IoT application simplification syntax, CommonJS extension syntax, a large number of IoT Builtin modules;
    * Includes application programming framework and device model for the IoT device side:
    * The device model is compatible with Ali's standard device model (attributes, events, methods);
    * Built-in cloud capability module on the device to interface with Ali's Link Platform IoT platform;
    * Supports board-level drivers, module and device driver dynamic loading capabilities, applications can be loaded locally and in the cloud;
    * Framework and JSE support multiple OS (OSAL), support AliOS, FreeRTOS;
    * Support multi-chip platform transplantation (HAL), has covered mainstream chips such as Qingke, Lexin, STM, MTK;
* Provide convenient local and cloud development tools:
    * The [be-cli](https://www.npmjs.com/package/be-cli) command line tool allows developers to develop and debug device-side applications locally;

* Provide a wealth of equipment, drivers, tools for device application development, modules:
    * Implement and open a large number of common device driver modules, scenario applications, for developers to quote;
    * Provide applications, device module download and distribution channels and capabilities;
    * Developers can submit modules for third-party applications;

JSEngine is suitable for developers of scene application development, device development, driver modules, etc. on the IoT device side.

### Framework

![](https://img.alicdn.com/tfs/TB18W2gkoT1gK0jSZFhXXaAtVXa-2387-1558.png)

### Samples

![](https://yuque.antfin-inc.com/api/filetransfer/images?url=https%3A%2F%2Fimg.alicdn.com%2Ftfs%2FTB1vO3JiET1gK0jSZFhXXaAtVXa-1024-720.gif&sign=99c20b6f96feb960affca12d0dd547e663ba79805bde75b996783a6580817fbd)

### Directories

```
.
├── docs                    # documents
├── scripts                 # javascript scripts, including demos, drivers, modules.
└── src
    ├── components          # components
    ├── engine              # JSEngine including duktapeJSE and liteJSE
    │   ├── duktape_engine
    │   └── lite_engine
    ├── include             # includes
    ├── main                # JSEngine Entry
    ├── port                # JSEngine port interface
    ├── services            # JSEngine service, like app manager, board manager, debugger etc.
    └── utils               # utils
```

### Building and Flashing firmware

**Building the firmware:**

```sh
# generate jsengine_app@yourboard default config
aos make jsengine_app@yourboard -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

> The currently validated boards are `mk3060`, `esp32devkitc`, `stm32f429zi-nucleo` and `developerkit`.

**Flashing the firmware into board:**

```sh
aos upload
```
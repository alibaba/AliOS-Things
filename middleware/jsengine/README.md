## Contents

## Introduction

JSEngine provides an application development framework, runtime environment, and development and deployment toolset for developing products in the JS language on the device side. It is designed to allow IoT device applications and developers to not pay too much attention to hardware platform differences, the underlying drivers and system implementation. In detail, you don't need to pay attention to the specific cloud device access technology, you can quickly develop IoT products, and can achieve the ultimate application cross-hardware platform running ability, truly "a code, run everywhere."

### Features

JSEngine has the following features:

* Provides a high-performance JS engine optimized for IoT application features:
    * Less resource usage: RAM < 15KB, ROM (Footprint) < 15KB;
    * CPU performance optimization: optimized lexical and syntactic parser, streamlined Javascript syntax support;
    * Complete JS syntax support: IoT application simplification syntax, CommonJS extension syntax, a large number of IoT Builtin modules;
    * Includes application programming framework and device model for the IoT device side:
    * The device model is compatible with Ali's standard device model (attributes, events, methods);
    * Built-in cloud capability module on the device to interface with Ali's Link Platform IoT platform;
    * Supports board-level drivers, module and device driver dynamic loading capabilities, applications can be loaded locally and in the cloud;
    * Framework and JSE support multiple OS (OSAL), support AliOS, FreeRTOS;
    * Support multi-chip platform transplantation (HAL), has covered mainstream chips such as Qingke, Lexin, STM, MTK;
* Provide convenient local and cloud development tools:
    * WebIDE supports local and cloud application code editing, debugging and publishing;
    * The be-cli command line tool allows developers to develop and debug device-side applications locally;

* Provide a wealth of equipment, drivers, tools for device application development, modules:
    * Implement and open a large number of common device driver modules, scenario applications, for developers to quote;
    * Provide applications, device module download and distribution channels and capabilities;
    * Developers can submit modules for third-party applications;

JSEngine is suitable for developers of scene application development, device development, driver modules, etc. on the IoT device side.

### Directories

### Building and Flashing

**Building the firmware:**

```sh
# generate jsengine_app@yourboard default config
aos make jsengine_app@yourboard -c config

# or customize config manually
aos make menuconfig

# build
aos make
```

> The currently validated boards are `mk3060`, `esp32devkitc`, and `developerkit`.

**Flashing the firmware into board:**

```sh
aos upload
```

### JSProgram debugging method

###方式一: Debugging with the be-cli tool.

####Step1: Tool installation

Execute ```make cli``` in the framework/JSEngine/tools directory to install and update the be_cli tool.

Note: The be_cli tool can also be installed manually (specify the version number): npm install be-cli@0.1.32 -g

* Refer to the be-cli tool usage guide .md in the docs/developer usage guide directory and update the device js program via be_cli.

####Step2: JS program updated to device

* There are multiple modules and app js example programs in the samples directory for reference.
* For example, we need to debug the gw example, using the following steps:
  * Use ```be -p samples/app/gw``` to package the js file in the gw directory into an app.bin.
  * Use the ```be connnect``` to connect the usb serial port of the module/development board.
  * Update app.bin to the device using ```be push app.bin```.
  * Restart the device, the file index.js will be loaded and run automatically after the device starts.

**Description**: If you encounter problems during the use of the be-cli tool, you can also use the second method to download the js file to the package.



###方式二:Package js file and download via make spifffs command

* Go to the ```framework/JSEngine/tools``` directory

* Copy your js program to the ```framework/JSEngine/tools/jspack``` directory and you must include a program called index.js.

  Description: The jspack directory has an index.js program by default, which implements helloworld printing via js console.log.

* Modified according to the hardware version you are currently using: PRODUCT_TYPE definition in ```framework/JSEngine/tools/jspack/Makefile```: developerkit by default.

* Execute ```make spiffs``` in the ```framework/JSEngine/tools``` directory; a successful execution will generate a spiffs.bin file.

* Finally execute the ```make download_js``` command to download the spiffs.bin file to the device.

* Finally power off and restart the device.

## Reference
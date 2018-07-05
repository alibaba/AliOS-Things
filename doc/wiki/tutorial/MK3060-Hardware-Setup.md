本文介绍庆科MK3060模组上AliOS Things硬件环境的搭建。

## mk3060模组
MK3060 是基于 ARM 核的 Wi-Fi 模组：

![](https://img.alicdn.com/tfs/TB1dkGJdwoQMeJjy0FoXXcShVXa-4160-2336.jpg)

## 串口线
MK3060 的串口已经通过miniusb接口接出，通过 miniusb-usb 线将串口与 PC 连接起来:

![](https://img.alicdn.com/tfs/TB16paGdwoQMeJjy0FnXXb8gFXa-4160-2336.jpg)

## 调试器
支持 Jlink 调试器:

![](https://img.alicdn.com/tfs/TB1bXEjg3MPMeJjy1XcXXXpppXa-4160-2336.jpg)

## 连接示例
连接好的硬件环境:

![](https://img.alicdn.com/tfs/TB1ThvGg3MPMeJjy1XdXXasrXXa-4160-2336.jpg)

# j-link烧写与调试
## openocd
j-link 烧写与调试，依赖于 [OpenOCD](https://files.alicdn.com/tpsservice/27ba2d597a43abfca94de351dae65dff.zip)，下载后解压，拷贝的AliOS Things源码的 build 目录下：

![](https://img.alicdn.com/tfs/TB1AVBOnMMPMeJjy1XcXXXpppXa-1003-466.png)

## j-link烧写
以 MK3060 为例，在编译的时候，带上jlink烧写参数：

`$aos make helloworld@mk3060 JTAG=jlink download`

会通过J-Link烧写固件到板子上。

## j-link调试
以MK3060为例，首先按住boot按键，不要松开，再按一下reset键，松开reset键，进入bootloader模式：

![](https://img.alicdn.com/tfs/TB1KjVFhgMPMeJjy1XcXXXpppXa-865-792.png)

**Window调试**  

在AliOS Things源码目录下面，分别运行 OpenOCD 命令和 gdb命令。

在一个 CMD 窗口下面启动OpenOCD：

 `$.\build\OpenOCD\Win32\openocd -f .\\build\OpenOCD\interface\jlink.cfg -f .\\build\OpenOCD\beken\beken.cfg -f .\\build\OpenOCD\beken\beken_gdb_jtag.cfg -l out\openocd_log.txt`

在另外一个 CMD 窗口，启动 gdb：

`$ arm-none-eabi-gdb -x=.gdbinit -ex 'target remote localhost:3333' ./out/eclipse_debug/last_built.elf --tui`

**Linux 调试**  

在AliOS Things源码目录下面，分别运行 OpenOCD 命令和 gdb命令。

在一个 Shell 窗口启动OpenOCD：

 `$ ./build/OpenOCD/Linux64/openocd  -f  build/OpenOCD/interface/jlink.cfg -f  build/OpenOCD/beken.cfg -f  build/OpenOCD/beken_gdb_jtag.cfg -l out/openocd_log.txt`

在另外一个 Shell 窗口启动 gdb：

`$ arm-none-eabi-gdb -x=.gdbinit -ex 'target remote localhost:3333' ./out/eclipse_debug/last_built.elf --tui`

gdb 里面设置断点，打印堆栈信息（MK3060最大支持两个断点）。



**AliOS-Things MasOS环境下开发跟Linux类似，只需要把Linux相关工具替换MacOS版本。**


# linux环境配置

可以尝试爱好者提供的一键安装脚本：[Setup Script for Linux/Mac](http://p28phe5s5.bkt.clouddn.com/setup_linux_osx.sh)。

## 依赖及aos-cube安装
以 Ubuntu 16.04 LTS (Xenial Xerus) 64-bit PC 版本为例，安装下列 pkg：
> 注意：请安装 python 2.7
```
$ sudo apt-get install -y python
$ sudo apt-get install -y gcc-multilib
$ sudo apt-get install -y libssl-dev libssl-dev:i386
$ sudo apt-get install -y libncurses5-dev libncurses5-dev:i386
$ sudo apt-get install -y libreadline-dev libreadline-dev:i386
$ sudo apt-get install -y python-pip
$ sudo apt-get install -y minicom
$ pip install --user -U aos-cube
```
## 交叉工具链

在 AliOS Things 源码的目录下面，运行：

```
$ aos make helloworld@mk3060
```

编译 mk3060 板子的 helloworld 示例程序。
根据所选的平台，会自动下载相应的工具链。

## minicom串口配置
配置串口参数（以MK3060为例），配置文件（/etc/minicom/minirc.dfl）内容 ：

```
pu port             /dev/ttyUSB0
pu baudrate         921600
pu bits             8
pu parity           N
pu stopbits         1
pu rtscts           No
```

启动 minicom，查看串口日志。

## 一些小技巧
Linux 操作串口及 j-link 会有root权限要求，尤其在使用 AliOS-Things Studio时候，对串口及 j-link 的权限需要先做配置：
- `$ sudo usermod -a -G dialout $(whoami)`,添加当前用户到 dialout 组，提供直接使用串口能力。
- `$lsubs` 找到 j-link 厂商ID。如：`Bus 002 Device 008: ID 1366:0105 SEGGER`,厂商ID为1366，新建`/etc/udev/rules.d/99-stlink-v2.rules`文件，在文件里面添加规则:`SUBSYSTEM=="usb", ATTR{idVendor}=="1366", MODE="666", GROUP="plugdev"`。

配置操作串口及 j-link权限后，重启系统生效。
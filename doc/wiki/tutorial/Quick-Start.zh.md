[EN](Quick-Start) | 中文

# 快速开始

本文通过直接在一台 linux 机器上运行 AliOS Things 来带您快速体验它。  
如果您在 Windows 或 Mac 上工作，也可以使用我们提供的[IDE](AliOS-Things-Studio)。


## 配置环境

您可以尝试一键安装脚本[Setup Script for Linux/Mac](http://p28phe5s5.bkt.clouddn.com/setup_linux_osx.sh),
或者按以下命令手动安装依赖的软件包
例：在一台 Ubuntu 16.04 LTS (Xenial Xerus) 64-bit PC 上
```bash
sudo apt-get install -y python
sudo apt-get install -y gcc-multilib
sudo apt-get install -y libssl-dev libssl-dev:i386
sudo apt-get install -y libncurses5-dev libncurses5-dev:i386
sudo apt-get install -y libreadline-dev libreadline-dev:i386
sudo apt-get install -y python-pip
sudo apt-get install -y minicom
```
## 安装 aos-cube
首先, 用 python 包管理器 `pip` 来安装 `aos-cube` 和`相关的依赖包`在全局环境，以便于后续使用 AliOS Things Studio 进行开发。
```bash
$ pip install setuptools
$ pip install wheel
$ pip install aos-cube
```
> 请确认`pip`环境是基于 Python 2.7 的。如果遇到权限问题，可能需要 `sudo` 来执行。

## 下载代码并编译运行

```bash
git clone https://github.com/alibaba/AliOS-Things.git
cd AliOS-Things
aos make helloworld@linuxhost
./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
```

## 效果

可以看见 `app_delayed_action` 在1秒时启动，每5秒触发一次。
```bash
$ ./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
 [   1.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [   6.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [  11.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [  16.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 ```

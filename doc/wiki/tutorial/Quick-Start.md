EN | [中文](Quick-Start.zh)

This guide offers a glance at AliOS Things, by running directly on a linux machine.  
If you are on Windows or Mac, maybe you'd like to turn directly to our [IDE](AliOS-Things-Studio).

## Setup environment

You can either try [Setup Script for Linux/Mac](http://p28phe5s5.bkt.clouddn.com/setup_linux_osx.sh), or manually do steps below,  
e.g. on a Ubuntu 16.04 LTS (Xenial Xerus) 64-bit PC

```bash
sudo apt-get install -y python
sudo apt-get install -y gcc-multilib
sudo apt-get install -y libssl-dev libssl-dev:i386
sudo apt-get install -y libncurses5-dev libncurses5-dev:i386
sudo apt-get install -y libreadline-dev libreadline-dev:i386
sudo apt-get install -y python-pip
sudo apt-get install -y minicom
```

## Install packages

It is recommended to install `aos-cube` and `relevant packages` globally, which helps developing with AliOS Things Studio in the future.
```bash
$ pip install setuptools
$ pip install wheel
$ pip install aos-cube
```
**`Note:`** Please make sure pip environment is based on Python 2.7 64bits. Use `sudo` if there's any permission issue.

```bash
if you want to upgrade aos-cube, please see below steps:

$ pip install --upgrade setuptools
$ pip install --upgrade wheel
$ pip install --upgrade aos-cube
```
**`Note:`** Please make sure `esptool, pyserial, scons` and `aos-cube` are installed sucessfully when run `pip install aos-cube`, or you can install them one by one if you meet problems.

## Run

```bash
git clone https://github.com/alibaba/AliOS-Things.git
cd AliOS-Things
aos make helloworld@linuxhost
./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
```

## Result

There you can see the delayed action starts in 1 sec and getting triggered every 5 secs.
```bash
$ ./out/helloworld@linuxhost/binary/helloworld@linuxhost.elf
 [   1.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [   6.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [  11.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [  16.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 ```

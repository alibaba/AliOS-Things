[EN](Starter-Kit-Tutorial) | 中文
## 步骤

### 下载/安装 Visual Studio Code
https://code.visualstudio.com/

### 安装 AliOS Studio 插件
1. 安装 C/C++ 插件
![](https://img.alicdn.com/tfs/TB1YVS4ghGYBuNjy0FnXXX5lpXa-3270-2182.png)
2. 安装 alios-studio 插件
![](https://img.alicdn.com/tfs/TB1eFS4ghGYBuNjy0FnXXX5lpXa-3270-2182.png)
> 如果已有安装，请确保 alios-studio 插件版本升级到 0.7.1 以上

### 开发环境准备

#### Linux/Mac

参考 [AliOS-Things-Linux-Environment-Setup](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Linux-Environment-Setup)

安装：
- [GNU ARM Embedded Toolchain](https://launchpad.net/gcc-arm-embedded/+download)
- [Python 2](https://www.python.org/downloads/)
- [Git](https://git-scm.com/downloads)

安装 python pip 包管理器，然后安装/升级 `aos-cube` 到全局环境:
`pip install -U aos-cube`
> aos-cube version >= 0.2.22

串口工具推荐安装：
minicom 或者 [picocom](https://github.com/npat-efault/picocom)

#### Windows
参考 [AliOS-Things-Windows-Environment-Setup](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Windows-Environment-Setup)  
可以直接使用我们准备好的软件包：http://p28phe5s5.bkt.clouddn.com/setup_windows.zip  
也可以手动安装以下软件/工具:  
- [GNU ARM Embedded Toolchain](https://launchpad.net/gcc-arm-embedded/+download)
- [Python 2](https://www.python.org/downloads/)
- [Git](https://git-scm.com/downloads)
- [ST-LINK USB Driver](http://www.st.com/en/development-tools/stsw-link009.html)

Python 2 和 Git 安装完成以后，在 Git Bash 中安装/升级 `aos-cube`:
`pip install -U aos-cube`
> aos-cube version >= 0.2.22

### 下载 AliOS Things 代码

从GitHub克隆：
`git clone https://github.com/alibaba/AliOS-Things.git`  
或者从镜像站点：
`git clone https://gitee.com/alios-things/AliOS-Things.git`


### 开始上手
#### 编译 helloworld

1. 在 Visual Studio Code 中打开下载好的 AliOS Things 代码目录
![](https://img.alicdn.com/tfs/TB1RJyMgf1TBuNjy0FjXXajyXXa-3270-2182.png)
![](https://img.alicdn.com/tfs/TB1v6uPgeuSBuNjy1XcXXcYjFXa-3270-2182.png)

2. 点击下方状态栏 ![](https://img.alicdn.com/tfs/TB1HC9KgkyWBuNjy0FpXXassXXa-72-22.png) 开始编译
编译过程如果发现缺少 toolchain 则会自动下载解压到正确位置。

> 但如果配置后仍有找不到toolchain的情况，请注意：  
> 可将 `arm-none-eabi-` 添加在全局PATH  
> 如果并未应用到全局PATH，则可以将toolchain拷贝到如下路径，也可在以下路径创建符号链接，确保如下路径有此 toolchain 即可  
> AliOS-Things/build/compiler/arm-none-eabi-5_4-2016q3-20160926/OSX/  
> AliOS-Things/build/compiler/arm-none-eabi-5_4-2016q3-20160926/Win32/  
> AliOS-Things/build/compiler/arm-none-eabi-5_4-2016q3-20160926/Linux64/  
```
../OSX
../Win32
../Linux64
├── arm-none-eabi/
├── bin/
├── lib/
└── share/
```

3. 编译完成后会显示各组件资源占用情况
![](https://img.alicdn.com/tfs/TB1q.wUgntYBeNjy1XdXXXXyVXa-3270-2182.png)

#### 烧录到 Starter Kit

1. 通过 USB Micro 线缆连接好 Starter Kit 与电脑
2. 点击下方状态栏 ![](https://img.alicdn.com/tfs/TB1TyipgbGYBuNjy0FoXXciBFXa-75-22.png) 完成固件烧录
![](https://img.alicdn.com/tfs/TB1jW2ngeuSBuNjSsplXXbe8pXa-3270-2182.png)

打开串口，可以看到 `app_delayed_action` 在1秒时启动，每5秒触发一次。
```
 [   1.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [   6.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [  11.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 [  16.000]<V> AOS [app_delayed_action#9] : app_delayed_action:9 app
 ```

### GUI 应用实例
双击左侧 `BUILD TARGET` 视图中的 `starterkitgui@starterkit` 开始编译 Starter Kit 的 GUI demo，将 Starter Kit 开发板与电脑通过 USB 连接好以后，点击 `Upload` 烧录固件。
![](https://img.alicdn.com/tfs/TB1RWo9gbSYBuNjSspiXXXNzpXa-3270-2182.png)

此 demo 读取加速度计实时数据，并绘制图表显示在 LCD 上
![](https://img.alicdn.com/tfs/TB1fuiEgwmTBuNjy1XbXXaMrVXa-1072-1458.png)

### 调试
#### 调试配置
点击左侧导航栏按钮切换到 `Debug` 标签页

![](https://img.alicdn.com/tfs/TB1pPlpguuSBuNjy1XcXXcYjFXa-3840-2400.png)

##### Mac/Linux
选择调试配置项 `Debug @ Mac/Linux`
![](https://img.alicdn.com/tfs/TB1ZzhpguuSBuNjy1XcXXcYjFXa-3840-2400.png)

根据已编译并烧录的`app@board`信息，更新 `AliOS-Things/.vscode/launch.json` 调试配置文件，比如：已编译并烧录 `starterkitgui@starterkit` 以后，更改相关配置
![](https://img.alicdn.com/tfs/TB1tRptgxSYBuNjSspjXXX73VXa-3840-2400.png)

##### Windows
在 Windows 中调试与 Mac/Linux 配置类似，区别是需要选中 `Debug @ Windows` 调试项，并将 `program` 路径修改为与已烧录固件对应的镜像文件，比如下图所示：
```
"program": "D:\\work\\AliOS-Things\\out\\starterkitgui@starterkit\\binary\\starterkitgui@starterkit.elf"
```
![](https://img.alicdn.com/tfs/TB114yFgCtYBeNjSspkXXbU8VXa-3840-2400.png)

#### 开始调试
点击左上方绿色三角▶️按钮(或F5)启动调试
![](https://img.alicdn.com/tfs/TB17HNpgAyWBuNjy0FpXXassXXa-3840-2400.png)

根据 `launch.json` 中的配置
```
"text": "break application_start"
```
启动调试以后会自动转到已设置的断点 `application_start` 函数处
同时上方会出现调试工具栏，提供常用的单步调试功能，如图所示：
![](https://img.alicdn.com/tfs/TB1tS8jgpuWBuNjSspnXXX1NVXa-3840-2400.png)

同样可以使用右键选择 `Run to Cursor`：
![](https://img.alicdn.com/tfs/TB1WDZ6gbGYBuNjy0FoXXciBFXa-3840-2400.png)

在左侧视图区，可以变量值进行观察：
![](https://img.alicdn.com/tfs/TB1JTZ6gbGYBuNjy0FoXXciBFXa-3840-2400.png)

#### 完成调试
至此，基本调试流程已走完，可以点击上方工具条红色停止键，结束调试。

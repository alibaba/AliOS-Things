## 步骤

### 下载/安装 Visual Studio Code
https://code.visualstudio.com/

### 安装 AliOS Studio 插件
1. 安装 C/C++ 插件
![](https://img.alicdn.com/tfs/TB1YVS4ghGYBuNjy0FnXXX5lpXa-3270-2182.png)
2. 安装 alios-studio 插件
![](https://img.alicdn.com/tfs/TB1eFS4ghGYBuNjy0FnXXX5lpXa-3270-2182.png)
> 如果已有安装，请确保 alios-studio 插件版本升级到 0.8.0 以上

### 开发环境准备

#### Linux/Mac

安装：
- [Python 2](https://www.python.org/downloads/)
- [Git](https://git-scm.com/downloads)

安装 python pip 包管理器，然后安装 `aos-cube` 到全局环境:
```
$ pip install --upgrade setuptools
$ pip install --upgrade wheel
$ pip install aos-cube
```

#### Windows
可以直接使用我们准备好的一键安装包：http://p28phe5s5.bkt.clouddn.com/setup_windows.zip  
也可以手动安装以下软件/工具:  
- [Python 2](https://www.python.org/downloads/)
- [Git (带 Git Bash)](https://git-scm.com/downloads)

Python 2 和 Git 安装完成以后，在 Git Bash 中安装 `aos-cube` 和`相关的packages`:
```
$ pip install --upgrade setuptools
$ pip install --upgrade wheel
$ pip install --user aos-cube
```

细节可参考 [AliOS-Things-Windows-Environment-Setup](https://github.com/alibaba/AliOS-Things/wiki/AliOS-Things-Windows-Environment-Setup) 

**`[注意]`** 请确定下在运行完 `pip install aos-cube`后`esptool, pyserial, scons` 和 `aos-cube` 全部被成功安装, 如果失败需要自行一步一步的安装这些依赖包.

```bash
如果只是升级aos-cube，可以按下述步骤:

$ pip install --upgrade setuptools
$ pip install --upgrade wheel
$ pip install --upgrade aos-cube
```

### 下载 AliOS Things 代码

从GitHub克隆：
`git clone https://github.com/alibaba/AliOS-Things.git`  
或者从国内镜像站点：
`git clone https://gitee.com/alios-things/AliOS-Things.git`


### 开始上手

这里我们基于 Starter Kit 开发板来介绍 AliOS Studio 的使用，其他板子可以此类推。  
对应视频 [使用 AliOS Studio 开始 AliOS Things 开发](http://v.youku.com/v_show/id_XMzU3OTE2MzI1Ng==.html)

#### 编译 helloworld

1. 在 Visual Studio Code 中打开下载好的 AliOS Things 代码目录
![](https://img.alicdn.com/tfs/TB1cPDapwmTBuNjy1XbXXaMrVXa-2032-1170.png)
2. 所有功能都集中在下方工具栏中  
小图标从左至右功能分别是 `编译` `烧录` `串口工具` `创建工程` `清除`  
![](https://img.alicdn.com/tfs/TB1WPzapwmTBuNjy1XbXXaMrVXa-286-22.png)  
左侧的 `helloworld@starterkit` 是编译目标，格式遵循 `应用名字@目标板名字` 的规则，点击它可以依次选择应用和目标板。  
![](https://img.alicdn.com/tfs/TB1Rt2gpwmTBuNjy1XbXXaMrVXa-2032-1170.png)
3. 编译目标确定以后，点击 ![](https://img.alicdn.com/tfs/TB1qR_UpuSSBuNjy0FlXXbBpVXa-24-22.png) 开始编译。  
编译过程如果发现缺少 toolchain 则会自动下载解压到正确位置。

#### 烧录到目标板

1. 通过 USB Micro 线缆连接好 Starter Kit 与电脑
2. 点击下方工具栏闪电图标完成固件烧录
![](https://img.alicdn.com/tfs/TB1YOvEprGYBuNjy0FoXXciBFXa-2032-1170.png)

#### 串口工具

点击下方工具栏插头图标打开串口，连接目标板，第一次连接会提示填写串口设备名和波特率，再次点击  
可以看到 `app_delayed_action` 在1秒时启动，每5秒触发一次，也可以在这里输入命令进行交互。  
**这里如果打开串口出错，请注意你的用户是否有串口访问权限**
![](https://img.alicdn.com/tfs/TB1CfefpTtYBeNjy1XdXXXXyVXa-2032-1170.png)

### 调试
细节参考 [Starter-Kit-Tutorial#调试](Starter-Kit-Tutorial#调试)  
或视频 [使用 AliOS Studio 开始 AliOS Things 调试](http://v.youku.com/v_show/id_XMzU3OTE5ODE1Ng==.html)

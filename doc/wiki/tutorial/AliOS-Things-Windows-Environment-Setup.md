# windows环境配置

可以尝试爱好者提供的一键安装脚本：[Setup Script for Windows](http://p28phe5s5.bkt.clouddn.com/setup_windows.zip)，下载解压后双击Install_Win即可完成全部依赖软件的安装。

## aos-cube安装
aos-cube 是 AliOS Things 在 Python 下面开发的项目管理工具包，依赖 Python 2.7 版本（在Python 2.7.14 版本上验证测试通过）。在 Python 官网下载对应的 2.7 版本的 [Python](https://www.python.org/downloads/) MSI 安装文件，安装时，选择 **pip** 和 **add python.exe to Path** 两个选项：

![](https://img.alicdn.com/tfs/TB1SlHyjqmgSKJjSspiXXXyJFXa-798-688.png)

注意：Python 请安装到不含空格的路径  
安装配置完成 Python 后，使用 pip 安装 aos-cube：

`> pip install aos-cube`

## 交叉工具链
Windows 工具链可以在链接 [GCC](https://launchpad.net/gcc-arm-embedded/+download) 下载 Windows的exe安装，勾选 Add path to environment variable 选项：

![](https://img.alicdn.com/tfs/TB16JofcWagSKJjy0FaXXb0dpXa-865-620.png)

在 AliOS Things 源码的目录下面，运行：

```
$ aos make helloworld@mk3060
```

编译 mk3060 板子的 helloworld 示例程序。

## 驱动安装
### 串口驱动
**FTDI系列**

FTDI驱动，在 [FTDI](http://www.ftdichip.com/Drivers/D2XX.htm) 下载 Windows 驱动程序并安装。

对应驱动安装完成后，连接设备，可在计算机 - 设备管理 - 端口，查看对应转换端口状态：

![](https://img.alicdn.com/tfs/TB1UziVXRTH8KJjy0FiXXcRsXXa-865-518.png)

驱动安装完成后，连接串口线，配置串口参数。在 MobaXterm Personal Edition 下，以MK3060为例，Session - Serial，选择端口，波特率为921600， Advanced Serial setting 里面，Software 选择 Minicom （allow manual COM port setting）：

![](https://img.alicdn.com/tfs/TB1tyqZXL2H8KJjy0FcXXaDlFXa-865-429.png)

对应参数配置好一会，点击 OK 按钮，查看串口日志。

### jtag驱动
**J-Link**

J-Link 驱动可在 [SEGGER J-Link驱动下载地址](https://www.segger.com/downloads/jlink/) 下，选择 J-Link Software and Documentation Pack，选择 J-Link Software and Documentation pack for Windows 下载安装：

![](https://img.alicdn.com/tfs/TB1s4L9bLNNTKJjSspfXXbXIFXa-865-466.png)

安装完成 J-Link 驱动后，需要安装 J-Link 的 libusb 驱动 [zadig](http://zadig.akeo.ie/)，下载后解压缩，运行 zadig.exe 或 zadig_xp.exe（Window XP系统）：

![](https://img.alicdn.com/tfs/TB1xyN2fgMPMeJjy1XcXXXpppXa-865-432.png)

选择 J-Link，driver 选择libusb-win32（v1.2.6.0），点击 Replace Driver，返回 successful 说明 libusb 安装成功：

![](https://img.alicdn.com/tfs/TB1OHStfgMPMeJjy1XcXXXpppXa-865-512.png)

关闭 zadig.exe，可在计算机 - 设备管理，查看 J-Link 识别状态：

![](https://img.alicdn.com/tfs/TB1XZcYaWagSKJjy0FhXXcrbFXa-865-568.png)

连接 J-Link 烧写或者调试。  

**如果此时无法法连接上 J-Link，或者烧写调试失败，在选择 J-Link 的 libusb 驱动选择时，尝试选择 libusbK (v3.0.7.0) （由于不同厂家的使用的 J-Link 固件版本不同，libusb-win32（v1.2.6.0）可能无法正常驱动J-Link）。**

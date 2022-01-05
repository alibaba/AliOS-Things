# 1、开发环境搭建


## 1.1 依赖条件


- 串口工具软件，查看日志用
```
Windows：Putty或者SecureCRT
Linux：minicom
macOS: Picocom
```

- 如果是mac电脑，依赖xcrun。可使用 `which xcrun` 查看是否存在，若不存在，使用下列命令安装。
```
xcode-select --install
```


- 如果是**windows**电脑，还需要手动安装GNU Make。从[官网](http://gnuwin32.sourceforge.net/downlinks/make.php)下载文件，并安装；然后将make路径添加到系统的环境变量Path里面。如下图所示：

![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/57453/1615778809297-b2b85c7f-a6ce-4a8a-a0d0-d31a74fe983f.png#crop=0&crop=0&crop=1&crop=1&height=437&id=uBvUx&margin=%5Bobject%20Object%5D&name=image.png&originHeight=874&originWidth=1466&originalType=binary&ratio=1&rotation=0&showTitle=false&size=409701&status=done&style=none&title=&width=733)
## 1.2 安装Miniconda

- macOS或者Linux，下载脚本并运行。

install_miniconda.sh

```shell
bash ./install_miniconda.sh
```

- Windows系统，下载脚本并双击运行。

install_miniconda.bat

- 如果使用脚本安装失败，可参考[手动安装Miniconda](#xRxRf)。
## 1.3 安装aos-tools


### 1.3.1 安装aos-tools、git和repo
####  1.3.1.1 macOS或者linux
**重新开一个新的终端**，使用以下命令创建一个名为tool_env的虚拟环境，并安装aos-tools，git，repo。如果安装过程较慢，可参考[安装慢的解决方案](#kxrn6)章节添加国内源，再执行以下命令。

```shell
conda create -y -n tool_env && conda activate tool_env && conda install -y pip \
    && pip install -i http://11.238.148.46:9002/ --trusted-host 11.238.148.46 -U aos-tools \
    && conda install -y git \
    && curl https://mirrors.tuna.tsinghua.edu.cn/git/git-repo -o repo && chmod +x repo \
    && mv repo $(dirname $(which python))/
```
#### 1.3.1.2 windows
打开miniconda命令行窗口![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/57453/1615555017092-569d24cb-9b6d-4369-8c05-6dfdb8256f55.png#crop=0&crop=0&crop=1&crop=1&height=28&id=jmlz4&margin=%5Bobject%20Object%5D&name=image.png&originHeight=46&originWidth=303&originalType=binary&ratio=1&rotation=0&showTitle=false&size=13846&status=done&style=none&title=&width=185)，输入以下命令安装aos-tools和git。如果安装过程较慢，可参考[安装慢的解决方案](#kxrn6)章节添加国内源，再执行以下命令。
```shell
# Windows10系统
conda create -y -n tool_env && conda activate tool_env && conda install -y pip && pip install -i http://11.238.148.46:9002/ --trusted-host 11.238.148.46 -U aos-tools && conda install -y git

# Windows7系统，已不支持最新版本python3.9，所以需要使用低版本的python
conda create -y -n tool_env python=3.8 && conda activate tool_env && conda install -y pip && pip install -i http://11.238.148.46:9002/ --trusted-host 11.238.148.46 -U aos-tools && conda install -y git
```
下载repo至虚拟环境中。
```shell
# 查询tool_env的安装路径，例如C:\Users\johnny\.conda\envs\tool_env
$ conda env list
# conda environments:
#
base                     C:\ProgramData\Miniconda3
tool_env              *  C:\Users\johnny\.conda\envs\tool_env
# 下载repo至虚拟环境的目录下
curl https://mirrors.tuna.tsinghua.edu.cn/git/git-repo > C:\Users\johnny\.conda\envs\tool_env\repo
echo @call python %~dp0\repo %* > C:\Users\johnny\.conda\envs\tool_env\repo.cmd
```
### 1.3.2 如何使用
**针对macOS或者Linux，重新开一个新的终端，**输入以下命令激活虚拟环境；
**针对Windows，打开miniconda命令行窗口**![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/57453/1615555017092-569d24cb-9b6d-4369-8c05-6dfdb8256f55.png#crop=0&crop=0&crop=1&crop=1&height=28&id=qaaBP&margin=%5Bobject%20Object%5D&name=image.png&originHeight=46&originWidth=303&originalType=binary&ratio=1&rotation=0&showTitle=false&size=13846&status=done&style=none&title=&width=185)，输入以下命令激活虚拟环境。
```shell
# 选择你需要激活的虚拟环境，如tool_env
$ conda activate tool_env
```
激活以后可以 `cd`到你的代码目录，去做编译，烧录等等操作。
# 2、代码下载
以下载helloworld_demo为例
```shell
aos create project -b haas100 -t helloworld_demo -d helloworld_demo  helloworld_demo

```
# 3、编译
以编译helloworld_demo为例，只需要执行aos make即可。
```shell
aos init (备注：仅首次编译需要aos init)
cd solutions/helloworld_demo

aos make -b haas100

编译成功之后，生成的固件位置在: ./hardware/chip/haas1000/release/write_flash_gui/ota_bin
```
注意，yaml是有比较严格的对齐等格式检查，建议使用vscode 的yaml插件![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/57453/1615535306467-26ad74af-668e-4a77-8082-6b0ee4c97077.png#crop=0&crop=0&crop=1&crop=1&height=40&id=B3miW&margin=%5Bobject%20Object%5D&name=image.png&originHeight=298&originWidth=302&originalType=binary&ratio=1&rotation=0&showTitle=false&size=29781&status=done&style=none&title=&width=41)进行编辑。
yaml语法请参考：[https://www.runoob.com/w3cnote/yaml-intro.html](https://www.runoob.com/w3cnote/yaml-intro.html)
# 4、烧录
haas100上电，连接usb线。如果无法识别串口的，请安装[驱动](https://cn.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)。
```shell
cd solutions/helloworld_demo
aos burn
```
选择烧录用的串口。如果烧录失败，可尝试换另一个CP2102N的串口试下。
![image.png](https://intranetproxy.alipay.com/skylark/lark/0/2021/png/57453/1615536230465-d44c77a0-9a9a-4e04-8086-256af14319a3.png#crop=0&crop=0&crop=1&crop=1&height=139&id=RGjGD&margin=%5Bobject%20Object%5D&name=image.png&originHeight=277&originWidth=1552&originalType=binary&ratio=1&rotation=0&showTitle=false&size=136233&status=done&style=none&title=&width=776)
对于Linux系统，访问串口需要sudo权限，请参考[网友文章](https://www.douban.com/note/542148407/)修改串口的权限。
```shell
sudo vim /etc/udev/rules.d/70-ttyusb.rules
# 增加如下内容：
KERNEL=="ttyUSB[0-9]*",MODE="0666"
```
保存，重新插入USB转串口，普通用户就可以访问串口了。
如果需要一次烧录多个文件，请参考《[烧录更多文件](#riXrl)》。

# 5、查看日志
在solution目录下使用，使用该命令打开串口。该命令成功执行之后，将在当前目录生成`.config_monitor`文件，保存串口号和波特率。下次可直接使用 `aos monitor` ，而无需再指定串口和波特率。
```
aos monitor [port] [baudrate]
```
设置好串口后， 重启设备就可以看到AliOS Things的启动log，和最后hello world的打印了。
```
hello world! count 2
hello world! count 3
```
# 6、附录
## 6.1 安装慢的解决方案
```shell
# 使用清华源
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.tuna.tsinghua.edu.cn/anaconda/pkgs/main/
conda config --set show_channel_urls yes
# 使用中科大源
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/free/
conda config --add channels https://mirrors.ustc.edu.cn/anaconda/pkgs/main/
conda config --set show_channel_urls yes
```
## 6.2 手动安装Miniconda
从官网下载安装包：[https://docs.conda.io/en/latest/miniconda.html](https://docs.conda.io/en/latest/miniconda.html)。
如果官网比较慢，也可以从USTC源下载安装包:[http://mirrors.ustc.edu.cn/help/anaconda.html](http://mirrors.ustc.edu.cn/help/anaconda.html)。
建议选择最新的python版本，当前为python3.8。安装包中会**自带python**，和你当前系统安装的python完全无关。
### 6.2.1 macOS或者Linux

- 安装

按照默认方式安装，license选择yes，目录用默认的，运行init选择yes。
```
#同意授权
Do you accept the license terms? [yes|no]
[no] >>> yes

#选择安装目录，使用默认
Miniconda3 will now be installed into this location:
/xxx/xxx/miniconda3
  - Press ENTER to confirm the location
  - Press CTRL-C to abort the installation
  - Or specify a different location below

#同意运行初始化脚本
by running conda init? [yes|no]
[no] >>> yes
```

- 确认安装结果

miniconda默认就创建一个名为base的虚拟环境，在此环境下，就可查看python是否指向刚刚安装的虚拟环境目录下/xxx/xxx/miniconda3。
```shell
(base) $ which python
/Users/johnny/miniconda3
```
如果不是指向miniconda3，请排查系统里面的设置，是否强制了python指向某个目录，如
`alias python='/usr/local/bin/python'`
### 6.2.2 Windows
按照默认方式安装，不要修改任何配置。
## 6.3 烧录更多文件
针对haas100，系统默认烧录编译出来的bin文件。如果想要烧录更多文件，如littlefs.bin，则需要使用program_data_files指定文件和地址。然后直接使用aos burn命令即可。
注意：**必须添加烧录起始地址**。
文件路径：hardware/chip/haas1000/package.yaml
```yaml
hw_info:
  arch_name: arm
  cpu_name: cortex-m4
  toolchain_prefix: arm-none-eabi
  vendor_name: alibaba IoT
  ld_script: _haas1000_alios_sec_b.lds
  flash_program: release/aos_burn_tool/flash_program.py
  # 在下面添加
  program_data_files:
    - filename: release/write_flash_tool/ota_bin/littlefs.bin
      address: 0xB32000
  # 如果还要烧录更多文件，可继续添加
    - filename: release/write_flash_tool/ota_bin/xxxx.bin
      address: 0xCCC000
```

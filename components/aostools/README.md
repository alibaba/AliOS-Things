# aos-tools

aos-tools 是 AliOS-Things 软件开发平台的命令行编译系统，通过 aos-tools 工具建立 AliOS-Things 开发环境，支持 AliOS-Things 组件管理、程序编译、烧录等功能。aos-tools 支持 Git 来获取 AliOS-Things 组件的代码，只是为了让您在 AliOS-Things 环境中更轻松地使用 Git，每个组件都是一个独立的 Git仓库，仍然支持使用 Git 命令来维护代码的版本管理。

## 安装 aostools

通过 pip 命令来安装 aos-tools 到你的系统中，aos-tools 支持 python2.7+、python3.6+。

```bash
sudo python -m pip install aos-tools
```

## aos-tools 命令

使用 aos-tools 需遵循的格式如下：

```bash
aos command options
```

可选元素显示在方括号 [ ] 中。例如，许多命令会用到`组件名`参数：

```bash
aos install [component0 component1 ... componentn]
aos list <-r>
```

### help

通过 aos 的 help 命令，可以查看 aos-tools 支持哪些命令，并可以获取该命令完整的使用说明。

```bash
aos help
```

可以通过在 aos 工作目录下运行以下命令来查看有关某个命令的详细信息：

```bash
aos help command
```

例如，以下命令会生成 aos `init` 参数的说明和选项列表，该参数会在当前目录中初始化 aos，创建 aos 工作目录。

```bash
aos help init
```

如果您仅想查看可用选项的列表，请运行：

```bash
aos command --help
```

或者

```bash
aos command -h
```

例如：

```bash
aos init --help
```

### init

通过 aos init 命令来初始AliOS Things 的开发环境：

```bash
mkdir aos_workspace
cd aos_workspace
aos init
```

执行完毕之后，会在`aos_workspace`目录生成一个隐藏的`.aos`文件，存储的内容为 `key: value` 形式键值对。内容类似于下文所述：

```json
repo: https://gitee.com/alios-things/manifest.git
branch: master
```

### search

在aos工作目录下使用。
根据关键字从云端查找相应的组件，并列出组件的简单描述。

```bash
aos search dummy
```

### install

```bash
aos install helloworld_demo
```

下载 `helloworld_demo` 示例工程，以及该所依赖的组件，每一个组件都对应一个 git 仓库，通过 `git clone` 命令将远程代码库中代码复制到本地目录。下载完组件后，当前的工作目录结构如下：

```bash
hardware/
components/
kernel
solutions/helloworld_demo/
```

### list

在 aos 工作目录下使用。显示单个或者多个组件的描述信息。
下面介绍几种典型场景的使用：

- 显示服务器所有的组件信息，并更新组件描述信息到本地数据库：

```bash
aos list -r
```

如果组件本地也存在，那么会在组件名字前显示 * 标记，例如:

```bash
* aos (V7.3.0)                            - aos kernel api
* at (V7.3.0)                             - AT 组件负责完成 AT 命令的发送、命令接收等整个 AT 命令数据交互流程。
  aui_cloud (V7.3.0)                      - 专为rtos打造的语音解决方案，提供了语音合成、语义理解、本地语义端文本理解等接口。
  av (V7.3.0)                             - av是一个轻量级的多媒体组件，主要包含AV基础框架、播放器和音频服务三部分。
  ble_mesh (V7.3.0)                       - ble_host and mesh stack
  board_dummy (V7.3.0)                    - csi_dummy development board configure.
  button (V7.3.0)                         - 按键服务组件,支持通过GPIO和ADC控制。
  cb2201 (V7.3.0)                         - CB2201 board configure.
```

- 显示本地所有组件信息

```bash
aos list
```

- 显示组件的依赖信息，包括依赖和被依赖的组件信息：

```bash
aos list aos -d
```

- 显示各种类型的组件信息，包括solution组件、chip组件、board组件、common组件：

```bash
aos list -s
aos list -c
aos list -b
aos list -m
```

### rename

在 aos 工作目录下使用。此命令不会对云端进行操作。一般是在用户基于某个已经存在的组件进行开发，当开发完毕时需要修改组件名字时用到。
运行此命令会修改以下几部分的内容：

- 文件夹名字
- `package.yaml`里面`name`字段
- 当被其他组件依赖时，在其他组件的`package.yaml`文件里的`depend`字段内容也会进行修改

### show

在 aos 工作目录下的solution类型组件下使用。用于显示方案依赖的组件列表、库(路径)、头文件、宏定义、变量、编译选项、链接选项、链接脚本、工具链、组件源码等信息。

当一个solution依赖单个或者多个board组件时，可以通过命令 `aos show -b board_name` 来查看方案依赖的组件列表和指定的board信息。

### toolchain

可以在任意目录下使用。可以安装csky的工具链，安装路径在`/usr/local/thead`下，并且会在`/usr/bin/`目录下实现软连接。

### export

在aos工作目录下使用。
此命令可以将aos工作目录下的单个组件或者多个组件以及这些组件所依赖的组件全部导出到指定的目录。

- 在solution类型组件下使用此命令可以导出所有类型组件到指定目录
- 在其他目录下使用此命令可以导出除solution组件外的其他组件到指定目录

```bash
aos export csi_pangu button /tmp/1
```

### format

在aos工作目录下使用此命令可以格式化组件的`package.yaml`文件，并保存到组件相应的目录中。

```bash
aos format component1
```

### variable

在solution类型组件目录下使用该命令可以将该solution的配置变量显示出来。比如：

```bash
BOARD_PATH = /home/xxx/work/git_project/yoc7.3/boards/csky/pangu_cpu0
BOARD      = PANGU_CPU0
CONFIG_TEE_CA = 1
CONFIG_CHIP_PANGU = 1
cpu_num    = 804
CHIP       = CSI_PANGU
CONFIG_DECODER_AMRNB = 1
ARCH       = CSKY
CPU        = CK804EF
CONFIG_DECODER_AMRWB = 1
CONFIG_DECODER_PCM = 1
CHIP_PATH  = /home/xxx/work/git_project/yoc7.3/components/csi_pangu
```

### convert

在aos工作目录下使用。
当用户想把一个源码目录快速变为一个 aos 组件时，可以使用这个命令。
执行以下命令会在`<dir>`目录下生成`package.yaml`文件模板，用户可根据需求再适当的调整修改`package.yaml`文件。

```bash
aos convert <dir>
```

### sdk

在solution类型目录下使用。
当编译完成一个solution之后，可以在当前目录使用`aos sdk`来生成sdk包；这个sdk包只有app部分是源码，其他组件都是以.a库的形式存在`aos_sdk`目录下。
这个过程中会自动把当前目录的`package.yaml`、`Makefile`、`.gitignore`这三个文件修改成适合sdk形式的文件。
可以将整个solution目录打包给需要的开发者使用。

### 工程编译

当您通过  `aos install helloworld`  命令下载了一个示例工程后，对应的工程代码及相关的组件代码已经安装到aos 工作环境中，可以通过以下命令，实现工程的编译：

```bash
cd solutions/helloworld
make
```

如果您是第一次使用 aostools ， `make` 命令会自动下载对应的编译工具链，并安装到系统中，您将看以下信息：

```bash
scons: Reading SConscript files ...
100.00% [##################################################] Speed: 6.787MB/S           
scons: done reading SConscript files.
scons: Building targets ...
```

### 注意事项

1、aos-tools依赖于python和pip，可基于python2或python3版本来使用。使用者需要保证两者处于同一大版本(注意：python有两个发展分支，分别为python2和python3)。可执行命令python --version和pip --version来判断
2、aos-tools仅支持 python2.7+、python3.6+。对于ubuntu16.04及以下版本，预装的python3版本均在3.6以下，若需要使用python3，请自行安装高版本python3环境，并建个软链接指向python。

### Q&A

1、linux环境下找不到pip或者安装时速度很慢如何办？

- 以ubuntu为例，通过修改/etc/apt/sources.list，可将源修改为mirrors.aliyun.com，然后执行sudo apt-get update。其他linux发行版源的修改，请自行搜索。
- 若开发者使用默认使用python2，ubuntu下可通过sudo apt-get install -y python-pip安装pip。若默认使用python3，则通过sudo apt-get install -y python3-pip安装。
- 安装后请执行pip2 --version或pip3 --version判断是否安装成功。
- 安装成功后，可通过建立软链接重新指向到pip(如果pip命令执行时还是找不到或已经存在)
- 可通过which命令获取到pip等命令安装的位置

2、系统中存在多个版本的python或pip怎么办，如何快速切换？

- 可通过update-alternatives命令来切换，具体使用方法请自行搜索。若发行版中不存在的话，可直接通过软链接的方式切换。

3、执行pip命令时发生错误怎么办?

- 请确认pip和python是否处于同一个大版本

4、ubuntu20.04下python2下无法安装pip怎么办？

- 首先将当前的python版本切换到python2
- 下载get-pip脚本：curl https://bootstrap.pypa.io/2.7/get-pip.py --output get-pip.py
- 执行命令安装：sudo python2 get-pip.py
- 判断是否安装成功pip2 --version，并可通过软链接指向pip




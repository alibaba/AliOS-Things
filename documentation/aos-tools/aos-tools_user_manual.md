@page aos-tools_user_manual 构建工具 aos-tools

[更正文档](https://gitee.com/alios-things/documentation/edit/rel_3.3.0/aos-tools/aos-tools_user_manual.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

aos-tools 是 AliOS Things 的命令行编译系统，支持组件管理、程序编译、烧录等功能。<br />aos-tools 使用git 来获取组件的代码和维护代码的版本管理。
# 安装 aos-tools
使用miniconda3创建一个虚拟环境，并在其虚拟环境中安装aos-tools。
```shell
pip install -U aos-tools
```
详细安装操作请参考《[HaaS100快速开始](https://g.alicdn.com/alios-things-3.3/doc/haas100_quick_start.html)》。
# aos-tools 命令
使用 aos-tools 需遵循的格式如下：
```
aos command options
```
可选元素显示在方括号 [ ] 中。例如，许多命令会用到`组件名`参数：
```
aos install [component0 component1 ... componentn]
aos list [-r]
```
## help
通过 aos 的 help 命令，可以查看 aos-tools 支持哪些命令，并可以获取该命令完整的使用说明。
```
aos help
```
可以通过在 aos 工作目录下运行以下命令来查看有关某个命令的详细信息：
```
aos help command
```
例如，以下命令会生成 aos `init` 参数的说明和选项列表，该参数会在当前目录中初始化 aos，创建 aos 工作目录。
```
aos help init
```
如果您仅想查看可用选项的列表，请运行：
```
aos command --help
```
或者
```
aos command -h
```
例如：
```
aos init --help
```
## init
通过 aos init 命令来初始AliOS Things 的工作区：
```
mkdir alios_iot
cd alios_iot
aos init
```
执行完毕之后，会在`alios_iot`目录生成一个隐藏的`.aos`文件，存储的内容为 `key: value` 形式键值对。内容为：
```
gitee_token: 32e702f14950e51e3a230e7ca8a60f10
occ_host: occ.t-head.cn
repo: https://gitee.com/alios-things/manifest.git
branch: master
```
如果使用定制化的manifest仓库(包含各个组件的类型、仓库url、版本等)，如git@gitlab.alibaba-inc.com:alios_iot/manifest.git，分支为v1.0.0，则使用一下命令初始化工作区：
```shell
aos init git@gitlab.alibaba-inc.com:alios_iot/manifest.git -b v1.0.0
```
## search
在aos工作目录下使用。 根据关键字从云端查找相应的组件，并列出组件的简单描述。
```
aos search rhino
```
## install
```
aos install helloworld_demo
```
下载 `helloworld_demo` 示例工程，以及该所依赖的组件，每一个组件都对应一个 git 仓库，通过 `git clone` 命令将远程代码库中代码复制到本地目录。下载完组件后，当前的工作目录结构如下：
```
components/
hardware/arch
hardware/board
hardware/chip
kernel/
solutions/helloworld_demo/
```
如果下载指定branch或tag的组件，则使用 `-b`选项，如下载v1.0.0分支。
```
aos install cli -b v1.0.0
```
如果从本地的组件压缩包，安装组件，则使用：
```
aos install -L xxx_v1.0.zip
```
## uninstall
删除单个组件，不修改其它组件的依赖关系。
```
aos uninstall component1
```
## list
在 aos 工作目录下使用。显示单个或者多个组件的描述信息。 下面介绍几种典型场景的使用：

- 显示服务器所有的组件信息，并更新组件描述信息到本地数据库：
```
aos list -r
```
如果组件本地也存在，那么会在组件名字前显示 * 标记，例如:
```
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
```
aos list
```

- 显示组件的依赖信息，包括依赖和被依赖的组件信息：
```
aos list rhino -d
```

- 显示各种类型的组件信息，包括solution组件、chip组件、board组件、common组件：
```
aos list -s
aos list -c
aos list -b
aos list -m
```
## rename
在 aos 工作目录下使用。此命令不会对云端进行操作。一般是在用户基于某个已经存在的组件进行开发，当开发完毕时需要修改组件名字时用到。 运行此命令会修改以下几部分的内容：

- 文件夹名字
- `package.yaml`里面`name`字段
- 当被其他组件依赖时，在其他组件的`package.yaml`文件里的`depend`字段内容也会进行修改
## copy
复制一个已有组件为一个新的组件。运行该命令会修改：

- 文件夹名字
- `package.yaml`里面`name`字段
## show
在 aos 工作目录下的solution类型组件下使用。用于显示方案依赖的组件列表、库(路径)、头文件、宏定义、变量、编译选项、链接选项、链接脚本、工具链、组件源码等信息。<br />当一个solution依赖单个或者多个board组件时，可以通过命令 `aos show -b board_name` 来查看方案依赖的组件列表和指定的board信息。
## toolchain
可以在任意目录下使用。可以安装csky和arm的工具链，安装路径在`~/.aliot`下。<br />可以使用`-d`选项删除已安装的toolchain。
## export
在aos工作目录下使用。 此命令可以将AliOS Things工作目录下的单个组件或者多个组件以及这些组件所依赖的组件全部导出到指定的目录。

- 在solution类型组件下使用此命令可以导出所有类型组件到指定目录
- 在其他目录下使用此命令可以导出除solution组件外的其他组件到指定目录
```
aos export csi_pangu button /tmp/1
```
## pack
在aos工作目录下使用。 此命令可以将AliOS Things工作目录下的单个组件，压缩成zip包后导出到指定的目录。该命令与 `aos install -L` 配合使用， `aos pack` 负责将组件打成压缩包，然后共享给其他开发者；其他开发者获取到该压缩包后，可使用`aos install -L`命令安装
```
aos pack component1 ../destdir
```
## upgrade
在aos目录下使用，将package.yaml里面的version作为仓库分支，更新组件代码。
## format
在aos工作目录下使用此命令可以格式化组件的`package.yaml`文件，并保存到组件相应的目录中。
```
aos format component1
```
## config
在solution类型组件目录下使用该命令可以将该solution的配置变量显示出来。比如：
```
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
## convert
在aos工作目录下使用。 当用户想把一个源码目录快速变为一个 AliOS Things 组件时，可以使用这个命令。 执行以下命令会在`<dir>`目录下生成`package.yaml`文件模板，用户可根据需求再适当的调整修改`package.yaml`文件。
```
aos convert <dir>
```


## make
当您通过 `aos install helloworld_demo` 命令下载了一个示例工程后，对应的工程代码及相关的组件代码已经安装到AliOS Things 工作环境中，可以通过以下命令，实现工程的编译：
```
cd solutions/helloworld_demo
aos make
```
如果您是第一次使用 aos-tools ， `aos make` 命令会自动下载对应的编译工具链，并安装到系统的`~/.aliot`目录，您将看以下信息：
```
scons: Reading SConscript files ...
100.00% [##################################################] Speed: 6.787MB/S           
scons: done reading SConscript files.
scons: Building targets ...
```
使用solution组件依赖的其它板子，如haas100，编译工程，则输入命令：
```
aos make -b haas100
```
使用`V=1`可以查看编译的详细信息。
```
aos make V=1
```
## make clean
在solution目录下使用，清除当前solution的编译信息。
```
aos make clean
```
## make distclean
在solution目录下使用，清除当前solution的编译信息。
```
aos make distclean
```
## burn
在solution目录下使用，使用aos make编译完成后，使用该命令烧录固件。烧录的参数配置，烧录工具，由当前solution使用的board组件或chip组件指定；烧录的文件，从solution组件的SConstruct文件中获取；如果SConstruct文件中未指定，则从当前solution的out目录下获取那个最新的`*@*.bin`格式的文件。
```
aos burn
```
如果是使用aos make -b <board_name>编译的话，则烧录固件时，也需要指定同样的板子。比如：
```
# 编译时指定板子haas100
aos make -b haas100
# 烧录固件时也必须指定同样的板子
aos burn -b haas100
```
成功烧录以后，将在当前目录生成`.config_burn`文件，保存串口号。如果下次烧录时，串口号有变化，则删除该文件后，重新烧录。
默认情况下，burn命令只烧录编译出来的bin文件。如果想要烧录更多文件，则需要在指定`flash_program`的那个`package.yaml`文件里，加上`program_data_files`字段。比如要额外烧录haas1000组件里`release/write_flash_tool/ota_bin/littlefs.bin`文件，则需要在haas1000组件的`package.yaml`里面，按照如下修改：
```
hw_info:
  flash_program: release/aos_burn_tool/flash_program.py
  # 以下为新增内容：在flash_program下面添加program_data_files，注意"-"号和对齐。
  program_data_files:
    - filename: release/write_flash_tool/ota_bin/littlefs.bin
      address: 0xB32000
```
## debug
在solution目录下使用，启动gdb server。配合Visual Studio Code的调试功能使用。
## monitor
在solution目录下使用，使用该命令打开串口。该命令成功执行之后，将在当前目录生成`.config_monitor`文件，保存串口号和波特率。下次可直接使用 `aos monitor` ，而无需再指定串口和波特率。
```
aos monitor [port] [baudrate]
```


## make solution@board -c config
为了兼容aos-cube的功能，保留该指令。<br />在aos顶层目录下使用，配置当前需要编译的solution和board。该命令将在当前目录下生成一个 `.config`文件。
```
aos make helloworld_demo@haas100 -c config
```
当配置完成以后，上述的`aos make` 、`aos make clean` 、`aos make distclean` 、`aos burn` 指令也随之升级，可以在aos顶层目录下使用。<br />另外，相比于`aos make clean`的功能， `aos make distclean` 命令将会额外执行删除`.config`文件的操作。
## create project
在非AliOS Things仓库目录下运行，创建工作区，并下载指定的solution作为模板解决方案，进行二次开发。
```
aos create project -b <board> -t <solution> -d <destdir> <solution_name>
```
其中 -b 选项，指定 开发板名称，如haas100；<br />-t 选项，指定 已有的解决方案名称，如http_demo。如果未设置，默认为helloworld_demo；<br />-d 选项，指定 工作区workspace目录，如果该目录指向已 **初始化过的工作区** ，则复用原来的工作区；<br />-r 选项，指定 远程manifest仓库地址，默认为 `https://gitee.com/alios-things/manifest.git`；<br />-B 选项，指定 远程manifest仓库的分支，默认为 `master`;<br />solution_name，指定新的solution名字，是将 -t选项 指定的解决方案 重命名。
```
# 根据模板helloworld_demo，创建一个新的解决方案mysolution
aos create project -b haas100 -t helloworld_demo -d /home/johnny/myworkspace mysolution
```
## sdk
在solution目录下使用，复制当前solution的所依赖组件的头文件和库文件至aos_sdk目录下。用户可将该solution目录提供给其他开发者，做二次开发。<br />


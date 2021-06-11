@page libc_stub libc_stub

[更正文档](https://gitee.com/alios-things/libc_stub/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
libc_stub组件是对C库桩函数（stubs）的实现，用于AliOS Things与C库的对接。支持gcc(newlibc)、icc、armcc三种类型编译器的C库。
对接的功能包括：
- 内存申请释放：malloc/free/realloc/calloc
- 错误码errno记录、异常abort
- 文件系统相关open/read/write/close等
- 输出printf

该组件只对接C库相关接口，posix标准接口请参考posix组件；文件系统相关请参考vfs组件。

## 版权信息
> Apache license v2.0

## 目录结构
```tree
├── newlib_stub.c # gcc newlibc库的重定向实现
├── compilers
│   ├── armlibc   # armcc库的重定向实现
│   ├── gcc       # gcc对接头文件修改
│   └── iar       # icc库的重定向实现
└── package.yaml  # 编译配置文件
```

## 依赖组件
* osal_aos
* vfs

# 常用配置
无

# API说明
参考标准C库接口即可。

# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件

由于libc_stub_example.c中要测试open文件系统相关接口，因此需要依赖文件系统，本示例以通用的ramfs为例，增加ramfs组件。
案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml

depends:
  - ramfs: master
  - libc_stub: master

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install ramfs libc_stub

```

上述命令执行成功后，组件源码则被下载到了./components/libc_stub路径中。

## 步骤4 添加示例
目前使用的gcc编译器都是使用的newlibc库，因此一般都要加上**libc_stub**组件。如果采用其他C库如musl库，则不需要添加。
在libc_stub组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/libc_stub/tree/master/example)：

```yaml
source_file:
  - "example/*.c"
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例
> CLI命令行输入：
```shell
libc_example
```

**关键日志**
> CLI日志：
```shell
libc_stub: malloc OK!
libc_stub: ramfs test success!
libc_stub comp test success!
```

# 注意事项
该模块只对接内核启动相关必要的接口，其他posix标准接口请参考posix组件；文件系统相关请参考vfs组件。

# FAQ



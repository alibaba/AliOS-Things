@page udisplay udisplay

**[更正文档](https://gitee.com/alios-things/udisplay/edit/master/README.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

# 概述

udisplay是AliOS Things一套显示接口组件，udisplay通过posix接口调用fbdev设备及backlight设备驱动，避免用户和LCD显示驱动直接交互，调用udisplay组件中的函数来进行framebuffer及背光调节相关操作。

## 版权信息
> Apache 2.0 License

## 目录结构
```tree
├── include
│   ├── udisplay.h            # 对外头文件
│   └── udispaly_cli.h        # 对外头文件
├── package.yaml              # 编译配置文件
├── src
│   └── udisplay.c            # udisplay核心处理层
└──example
    └── udisplay_example.c    # udisplay测试用例
```

## 依赖组件

* osal_aos
* fbdev

# 常用配置
```yaml
def_config:                              # 组件的可配置项
  AOS_COMP_UDISPLAY: 1                   # 使能组件宏定义
```

# API说明
- 参考 [udisplay_aos_api](https://g.alicdn.com/alios-things-3.3/doc/group__udisplay__aos__api.html)
- 参考 [udisplay_cli_aos_api](https://g.alicdn.com/alios-things-3.3/doc/group__udisplay__cli__aos__api.html)

# 使用示例

组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具 **alios-studio** ，所以首先需要参考[《aos-studio使用说明之搭建开发环境》](https://g.alicdn.com/alios-things-3.3/doc/setup_env.html)，下载安装 **alios-studio** 。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《aos-studio使用说明之打开工程》](https://g.alicdn.com/alios-things-3.3/doc/open_project.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《aos-studio使用说明之创建工程》](https://g.alicdn.com/alios-things-3.3/doc/create_project.html)。

## 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对组件的依赖：

```yaml

depends:
  - udisplay: master  # helloworld_demo 中引入udisplay组件
  - ili9341: master   # helloworld_demo 中引入ili9341屏幕驱动

def_config:
    CLI_IOBOX_ENABLE: 1        # 可选：使能文件系统测试命令，例如ls, ll, mkdir, touch, echo等

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install udisplay

```

上述命令执行成功后，组件源码则被下载到了./components/udisplay路径中。

## 步骤4 添加示例
在udisplay组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/udisplay/tree/master/example)：

```yaml
source_file:
  - "example/udisplay_example.c" # add udisplay_example.c
```

**LCD连线**
使用本测试示例时，请先连接SPI LCD屏幕。

LCD购买链接[https://item.taobao.com/item.htm?spm=a1z09.2.0.0.768d2e8d9D3S7s&id=38842179442&_u=m1tg6s6048c2](https://item.taobao.com/item.htm?spm=a1z09.2.0.0.768d2e8d9D3S7s&id=38842179442&_u=m1tg6s6048c2)

请选择购买2.4寸屏。

HaaS100开发板请务必使用V1.1以上的版本，HaaS100扩展口：

<div align=left display=flex>
    <img src="https://img-blog.csdnimg.cn/img_convert/b18b27ec7957e010d0380e76ebb33e98.png" style="max-width:800px;" />
</div>

LCD与HaaS100接线对应pin脚：

<div align=left display=flex>
    <img src="https://img-blog.csdnimg.cn/img_convert/5b2c8c014991f028978f5743ba22d193.png" style="max-width:800px;" />
</div>

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

上述步骤执行后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

测试步骤：
```shell
$ udisplay_init  # 初始化udisplay组件
$ udisplay pattern 16 0x0000  # 绘制pattern颜色为黑色
```

> udisplay 初始化命令：
```shell
udisplay_init
```

> CLI命令行输入：
```shell
udisplay pattern 16 0x0000
```

+ pattern 是测试pattern固定字符.
+ 16: ili9341 屏幕为 RGB565
+ 0x0000  RGB565 颜色设置

# FAQ

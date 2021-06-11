@page ugraphics ugraphics

**[更正文档](https://gitee.com/alios-things/ugraphics/edit/master/README.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

# 概述
ugraphics组件是基于SDL2封装的接口，支持JPEG/PNG图像解码绘制、点/线/框图像等绘制、图像旋转、RGB格式转换、中英文字符绘制等功能，方便用户进行基础的图像应用开发。

## 版权信息
> Apache 2.0 License

## 目录结构
```tree
├── include
│   ├── ugraphics.h                 # 对外头文件
│   ├── format
│   │    └── to_rgb565.h            # rgb格式转换头文件
│   └── rotate
│        └── rotate.h               # rotate旋转头文件
├── package.yaml                    # 编译配置文件
├── src
│   ├── ugraphics.c                 # 图像操作函数代码
│   ├── format
│   │    └── to_rgb565.c            # rgb格式转换函数代码
│   ├── jpegdec
│   │    └── jpegdec.c              # 基于libjpeg实现的图像解码实现
│   └── rotate
│        └── rotate.c               # 图像旋转函数代码
└── example
    ├── image                       # 测试用例图片资源文件
    └── ugraphics_example.c         # 测试用例
```

## 依赖组件

* SDL2
* littlefs

# 常用配置
```yaml
def_config:                              # 组件的可配置项
  AOS_COMP_JPEG: 0                       # 使能jpeg decoder函数接口
  CONFIG_UGRAPHICS_FORMAT: 0             # 使能format函数接口
  CONFIG_UGRAPHICS_ROTATE: 0             # 使能rotate旋转函数接口
```

> 配置是否支持JPEG解码、图像格式转换、图像旋转，默认是关闭的

# API说明
- 参考 [ugraphics_aos_api](https://g.alicdn.com/alios-things-3.3/doc/group__ugraphics__aos__api.html)

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
  - ugraphics: master            # helloworld_demo 中引入ugraphics组件
  - ili9341: master              # helloworld_demo 中引入ili9341屏幕驱动

def_config:
    CLI_IOBOX_ENABLE: 1        # 可选：使能文件系统测试命令，例如ls, ll, mkdir, touch, echo等

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install ugraphics

```

上述命令执行成功后，组件源码则被下载到了./components/ugraphics路径中。

## 步骤4 添加示例

在ugraphics组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/ugraphics/tree/master/example)：

```yaml
source_file:
  - "example/ugraphics_example.c" # add ugraphics_example.c

build_config:
 prebuild_script: cp_resources.py  #编译时cp_resources.py会对资源文件进行拷贝，系统自动打包资源文件到littlefs文件系统中

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

**文件系统烧录**
本组件例子中使用到到图片存放在代码中hardware/chip/haas1000/prebuild/data/目录下ugraphics_image目录，除烧录helloworld_demo image外，需烧录littlefs文件系统，请将hardware/chip/haas1000/package.yaml文件中以下代码段的注释打开：

```yaml
  program_data_files:
    - filename: release/write_flash_tool/ota_bin/littlefs.bin
      address: 0xB32000
```

上述步骤执行后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

测试步骤：
```shell
$ ugraphics init # 初始化ugraphics
$ ugraphics draw {rect|jpg|png|line|string...} # 绘制图像
```

测试步骤：
```sh
$ ugraphics init # 初始化ugraphics
$ ugraphics draw {rect|jpg|png|line|string...} # 绘制图像

### CLI命令行输入：
```shell
ugraphics init # 资源初始化
```

> CLI关键日志：
```shell
ugraphics init ok!
```

**CLI命令行输入：**
```shell
ugraphics draw rect # 绘制矩形框
```

> CLI关键日志：
```shell
ugraphics draw rectangle ok!
```

**CLI命令行输入：**
```shell
ugraphics draw jpg # 绘制jpeg图片
```

> CLI关键日志：
```shell
ugraphics draw jpg image ok!
```

**CLI命令行输入：**
```shell
ugraphics draw png # 绘制png图片
```

> CLI关键日志：
```shell
ugraphics draw png image ok!
```

**CLI命令行输入：**
```shell
ugraphics draw line # 绘制线条
```

> CLI关键日志：
```shell
ugraphics draw line ok!
```

**CLI命令行输入：**
```shell
ugraphics draw string # 绘制字符串
```

> CLI关键日志：
```shell
ugraphics draw string ok!
```

**CLI命令行输入：**
```shell
ugraphics fill rect # 填充矩形框
```

> CLI关键日志：
```shell
ugraphics fill rectangle ok!
```

**CLI命令行输入：**
```shell
ugraphics clear # 清屏
```

> CLI关键日志：
```shell
ugraphics clear screen ok!
```

**CLI命令行输入：**
```shell
ugraphics quit # 释放资源
```

> CLI关键日志：
```shell
ugraphics quit ok!
```

# 注意事项
NA

# FAQ
NA



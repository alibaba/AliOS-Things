@page jsoncpp jsoncpp

[更正文档](https://gitee.com/alios-things/jsoncpp/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述

JSON是一种轻量级的数据交换格式。它可以表示数字，字符串，值的有序序列以及名称/值对的集合。

JsonCpp是一个C ++库，它允许处理JSON值，包括在字符串之间进行序列化和反序列化。它还可以在反序列化/序列化步骤中保留现有注释，从而使其成为存储用户输入文件的便捷格式。

JsonCpp 是一个开源三方库，官方地址如下：
https://github.com/open-source-parsers/jsoncpp

该组件支持以下功能：
- json字符串编码
- json字符串解码

## 版权信息
> MIT license

## 目录结构
```tree
.
├── cmake
├── CMakeFiles
├── devtools
├── doc
├── example
│   └── jsoncpp_comp_example.c    # jsoncpp 测试用例代码
├── include
│   └── json                      # jsoncpp 头文件
├── pkg-config
├── src
│   └── lib_json                  # jsoncpp 核心源文件
└── test                          # jsoncpp 测试相关文件
```

## 依赖组件

* osal_aos
* cplusplus

# 常用配置

# API说明
    第三方库 API 对应放在头文件中，每个头文件的类都包含了类中函数的详细解释

Read 相关类：

```C
class JSON_API CharReader
class JSON_API Factory
class JSON_API CharReaderBuilder
```

Write 相关类：

```C
class JSON_API StreamWriter
class JSON_API StreamWriterBuilder
```
value 相关类：

```C
class JSON_API Value
class JSON_API Path
class JSON_API ValueIteratorBase
class JSON_API ValueConstIterator
class JSON_API ValueIterator 
```

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
  - jsoncpp: master # helloworld_demo中引入jsoncpp组件

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install jsoncpp

```

上述命令执行成功后，组件源码则被下载到了./components/jsoncpp路径中。

## 步骤4 添加示例

在jsoncpp组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/jsoncpp/tree/master/example)：

```yaml
source_file:
  - "src/lib_json/json_reader.cpp"
  - "src/lib_json/json_value.cpp"
  - "src/lib_json/json_writer.cpp"
  - "example/jsoncpp_comp_example.cpp" # add jsoncpp_comp_example.cpp
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
```sh
jsoncpp_write_string_example
```
> CLI命令行显示结果：
```sh
json write json_file = {
        "action" : "run",
        "data" : 
        {
                "number" : 1
        }
} !
```

> CLI命令行输入：
```sh
jsoncpp_comp_read_string_example
```
> CLI命令行显示结果：
```sh
json read name = colin !
josn read age = 20 !
```


# FAQ

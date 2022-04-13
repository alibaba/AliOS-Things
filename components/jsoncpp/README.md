@page jsoncpp jsoncpp


# 概述

JSON是一种轻量级的数据交换格式。它可以表示数字，字符串，值的有序序列以及名称/值对的集合。

JsonCpp是一个C ++库，它允许处理JSON值，包括在字符串之间进行序列化和反序列化。它还可以在反序列化/序列化步骤中保留现有注释，从而使其成为存储用户输入文件的便捷格式。

JsonCpp 是一个开源三方库，官方地址如下：
https://github.com/open-source-parsers/jsoncpp

该组件支持以下功能：
- json字符串编码
- json字符串解码

# 版权信息
> MIT license

# 目录结构
```sh
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

# 依赖组件

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
示例代码参考example/jsoncpp_comp_example.cpp，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> jsoncpp组件的package.yaml中添加
```C
source_file:
  - "src/lib_json/json_reader.cpp"
  - "src/lib_json/json_value.cpp"
  - "src/lib_json/json_writer.cpp"
  - "example/jsoncpp_comp_example.cpp" # add jsoncpp_comp_example.cpp
```

## app中添加jsoncpp组件
> helloworld_demo组件的package.yaml中添加
```C
depends:
  - jsoncpp: dev_aos # helloworld_demo中引入jsoncpp组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```


## 烧录固件
> 参考具体板子的快速开始文档。

## jsoncpp示例测试
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

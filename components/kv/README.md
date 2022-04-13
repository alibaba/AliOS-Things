@page kv kv

# 概述
对于嵌入式系统应用中，频繁使用的参数存储，过程变量存储等操作，AliOS-Things为用户提供了一种更加直观易于理解的基于键值对的存储方式，如报警温度=50度，可以通过定义一个键值对：{"AlarmTemp":50}来进行存储。获取时，也只需通过AlarmTemp这个关键字（键/KEY)即可快速获取和重新写入。KV将用户的指定片内flash扇区或eeprom虚拟为有一个存储空间，并且帮助用户管理键值对之间的映射关系，用户无需关心具体的值被写到哪里了，仅需要通过对键的操作即可完成数据的存取。该组件目前针对Nor flash设备。
组件支持以下功能：
- key-value pair set
- key-value pair get
- key-value pair delete
- key-value pair delete for group (same prefix)

## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── src
│   ├── kv_adapt.c   # kv和flash的适配层
│   ├── kv_aos.c     # kv的aos api接口
│   ├── kv.c         # kv的核心实现代码
│   ├── kv_cli.c     # kv cli注册命令
│   └── kv_secure.c  # kv的安全存储适配层
├── include
│   ├── aos
│   │   └── kv.h     # kv的对外AOS API
│   └── kv_api.h     # kv_开头的API
├── internal         # 内部头文件
├── package.yaml     # 编译配置文件
└── example
    └── kv_example.c # kv示例代码
```

## 依赖组件
* rhino
* cli

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在yaml中**def_config**节点修改，具体如下：
> kv分区大小: 默认8K bytes, 如需修改，在yaml中修改KV_CONFIG_TOTAL_SIZE配置
```sh
def_config:
  KV_CONFIG_TOTAL_SIZE: 4096
```
> kv分区号，默认HAL_PARTITION_PARAMETER_2，可按照实际需要修改yaml配置如：
```sh
def_config:
  KV_CONFIG_PARTITION: 2
```
> kv block bit大小，默认12，可修改yaml配置如：
```sh
def_config:
  KV_CONFIG_BLOCK_SIZE_BITS: 2
```
> kv 资源回收任务优先级，默认32，可修改yaml配置如：
```sh
def_config:
  KV_CONFIG_TASK_PRIORITY: 31
```
> kv 资源回收任务栈大小，默认1024，可修改yaml配置如：
```sh
def_config:
  KV_CONFIG_TASK_STACK_SIZE: 2048
```
> kv 存储key最大长度，默认为128，可修改yaml配置如：
```sh
def_config:
  KV_CONFIG_MAX_KEY_LEN: 256
```
> kv 存储value最大长度，默认为512，可修改yaml配置如：
```sh
def_config:
  KV_CONFIG_MAX_VAL_LEN: 256
```

# API说明
@ref kv_aos_api

# 使用示例
示例代码参考example/kv_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> kv组件的package.yaml中添加example
```sh
source_file:
  - "src/*.c"
  - "example/kv_example.c" # add kv_example.c
```

## 添加kv组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - kv: master # helloworld_demo中引入kv组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```
其中具体单板还需要先配置环境：
```sh
aos make helloworld_demo@haas100 -c config
```

## 烧录固件
> 参考具体板子的快速开始文档。

## KV示例测试
> CLI命令行输入：
```sh
kv_example
```

## 关键日志
> CLI日志：
```sh
kv comp test success!
```

# 注意事项
如果需要使用kv的安全加解密(mbedtls version)，就需要对接两个接口：**kv_secure_get_key** and **kv_secure_get_iv**。(declare in `kv_adapt.h`)
Example:
```C
uint8_t aes_key[32] = {
        0x86, 0xf6, 0xd2, 0xbe, 0x45, 0xb5, 0xab, 0x9c,
        0xc7, 0xd5, 0x96, 0xf7, 0xaf, 0x45, 0xfa, 0xf7,
        0xbe, 0x6a, 0x5d, 0xb0, 0x04, 0xc4, 0xde, 0xb5,
        0xf5, 0x0c, 0x4f, 0xc3, 0x71, 0x19, 0x3e, 0xe8
    };

uint8_t aes_iv[16]  = {
        0xef, 0x80, 0x18, 0xdc, 0xa3, 0x72, 0x72, 0x31,
        0x99, 0x2e, 0x3a, 0xba, 0x60, 0xf5, 0x0b, 0xd4
    };

uint8_t* kv_secure_get_key(uint32_t len)
{
    if ((len <= 0) || (len > sizeof(aes_key))) {
        return NULL;
    }

    return aes_key;
}

uint8_t* kv_secure_get_iv(uint32_t len)
{
    if ((len <= 0) || (len > sizeof(aes_iv))) {
        return NULL;
    }

    return aes_iv;
}
```

# FAQ
Q1： 在不知道key值的情况下，可以遍历kv分区吗？
> 答：可以通过串口输入cli命令**kv list**遍历输出所有的kv值。


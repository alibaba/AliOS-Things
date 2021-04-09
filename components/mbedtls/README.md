@page mbedtls mbedtls

# 概述
mbedtls提供加解密算法（AES, RSA, MD5/SHA1/SHA256/SHA512 etc.），X.509证书管理和TLS/DTLS协议支持。详细介绍可参考[mbedtls官网](https://tls.mbed.org/)。
组件支持以下功能：
- TLS(1.0, 1.1, 1.2), DTLS(1.0, 1.2)
- X.509证书
- 加解密算法(DES/3DES/AES/RSA/ARIA/ARC4/BLOWFISH/CAMELLIA/XTEA/CHACHA20/POLY1305)
- 摘要算法(MD5/SHA1/SHA256/SHA512/RIPEMD160)

## 版权信息
> Apache license v2.0

## 目录结构
```sh
|-- apache-2.0.txt
|-- ChangeLog
|-- example              #适配过AliOS Things的示例代码
|-- include              #mbedtls头文件
|-- library              #mbedtls实现代码
|-- LICENSE              #License申明
|-- package.yaml         #编译和配置文件
|-- platform             #适配不同平台的代码，目前支持AliOS Things和yoc
|-- programs             #开源原生的示例代码
|-- README.mbedtls.md    #开源原生的README文档
|-- README.md            #README文档
```

## 依赖组件
* osal_aos
* netmgr

# 常用配置
系统中相关配置已有默认值，如需修改配置，统一在platform/include/config_aos_base.h中修改，具体如下：
> 使能加解密: 默认使能, 如需关闭，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO: 0
```
> 使能MD5: 默认使能，如需关闭，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_MD5: 0
```
> 使能SHA1: 默认使能，如需关闭，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_SHA1: 0
```
> 使能SHA256: 默认使能，如需关闭，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_SHA256: 0
```
> 使能SHA512: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_SHA512: 1
```
> 使能RIPEMD160: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_RIPEMD160: 1
```
> 使能ARIA: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_ARIA: 1
```
> 使能DES: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_DES: 1
```
> 使能AES: 默认使能，如需关闭，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_AES: 0
```
> 使用AES ROM表: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_AES_ROM_TABLES: 1
```
> 使用较少的AES ROM/RAM表: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_AES_FEWER_TABLES: 1
```
> 使能CBC加密模式: 默认使能，如需关闭，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_CBC: 0
```
> 使能CFB加密模式: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_CFB: 1
```
> 使能CTR加密模式: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_CTR: 1
```
> 使能OFB加密模式: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_OFB: 1
```
> 使能XTS加密模式: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_XTS: 1
```
> 使能GCM加密模式: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_GCM: 1
```
> 使能CCM加密模式: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_CCM: 1
```
> 使能ARC4加密: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_ARC4: 1
```
> 使能BLOWFISH加密: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_BLOWFISH: 1
```
> 使能CAMELLIA加密: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_CAMELLIA: 1
```
> 使能XTEA加密: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_XTEA: 1
```
> 使能CHACHA20加密: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_CHACHA20: 1
```
> 使能POLY1305 MAC算法: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_POLY1305: 1
```
> 使能CHACHAPOLY AEAD算法: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_CRYPTO_CHACHAPOLY: 1
```
> 使能X.509证书支持: 默认使能，如需关闭，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_X509: 0
```
> 使能TLS协议支持: 默认使能，如需关闭，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_TLS: 0
```
> 使能TLS1.2协议支持: 默认使能，如需关闭，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_TLS1_2: 0
```
> 使能DTLS协议支持: 默认使能，如需关闭，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_DTLS: 0
```
> 使能TLS协议调试支持: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_TLS_DEBUG: 1
```
> TLS/DTLS最大收发报文长度: 单位：字节，默认为4096，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_TLS_MAX_CONTENT_LEN: 2048
```
> 是否包含自测程序: 默认不包含，如需包含，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_SELFTEST: 1
```
> 使能错误字符串支持: 默认不使能，如需使能，可修改配置如：
```sh
def_config:
  MBEDTLS_CONFIG_ERROR: 1
```

# API说明
mbedtls API用法与开源版本一致，API用法可以参考[mbedtls官网API说明](https://tls.mbed.org/api/), 也可以在网上搜索其API的用法。

# 使用示例
mbedtls提供了TLS/DTLS协议，加解密算法等众多功能，其在programs目录下也提供了各功能的示例代码，注意这些示例代码虽然不能直接在
AliOS Things上运行起来，但其逻辑是可以参考的。我们也在example目录下提供了可以直接在AliOS Things运行的示例。
示例代码参考example/tls_example.c，以运行helloworld_demo为例，具体步骤如下：

## 添加示例代码
> mbedtls组件的package.yaml中添加example
```sh
source_file:
  - 原有其他文件
  - "example/tls_example.c" # add tls_example.c
```

## app中添加mbedtls组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - mbedtls: master # helloworld_demo中引入mbedtls组件
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

## mbedtls示例测试
tls_example的测试依赖于网络，在进行测试前，首先执行联网命令，请先阅读netmgr组件的README.md
> CLI命令行输入：
```sh
netmgr_example
netmgr -t wifi -c <ssid(你的wifi名)> <passworld(你的wifi密码)>
tls_example
```
注意这里使用2.4GHZ的wifi网络。

## 关键日志
> CLI日志：
```sh
tls_example test success!
```

# 注意事项
无

# FAQ

Q1： TLS 握手失败，Debug消息显示"buffer too small ..." 或者 "bad message length"

答：这可能是TLS消息出来的I/O buffer小于消息的长度， 通过配置增大I/O buffer即可。
修改yaml配置文件中MBEDTLS_CONFIG_TLS_MAX_CONTENT_LEN。
AliOS Things默认配置的I/O buffer为4KB， 一般扩大到16384 （16KB）可以解决所有的此类问题。

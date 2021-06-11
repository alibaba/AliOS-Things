@page mbedtls mbedtls

[更正文档](https://gitee.com/alios-things/mbedtls/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

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
```tree
mbedtls
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
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO: 0
```
> 使能MD5: 默认使能，如需关闭，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_MD5: 0
```
> 使能SHA1: 默认使能，如需关闭，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_SHA1: 0
```
> 使能SHA256: 默认使能，如需关闭，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_SHA256: 0
```
> 使能SHA512: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_SHA512: 1
```
> 使能RIPEMD160: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_RIPEMD160: 1
```
> 使能ARIA: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_ARIA: 1
```
> 使能DES: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_DES: 1
```
> 使能AES: 默认使能，如需关闭，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_AES: 0
```
> 使用AES ROM表: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_AES_ROM_TABLES: 1
```
> 使用较少的AES ROM/RAM表: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_AES_FEWER_TABLES: 1
```
> 使能CBC加密模式: 默认使能，如需关闭，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_CBC: 0
```
> 使能CFB加密模式: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_CFB: 1
```
> 使能CTR加密模式: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_CTR: 1
```
> 使能OFB加密模式: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_OFB: 1
```
> 使能XTS加密模式: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_XTS: 1
```
> 使能GCM加密模式: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_GCM: 1
```
> 使能CCM加密模式: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_MODE_CCM: 1
```
> 使能ARC4加密: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_ARC4: 1
```
> 使能BLOWFISH加密: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_BLOWFISH: 1
```
> 使能CAMELLIA加密: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_CAMELLIA: 1
```
> 使能XTEA加密: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_XTEA: 1
```
> 使能CHACHA20加密: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_CHACHA20: 1
```
> 使能POLY1305 MAC算法: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_POLY1305: 1
```
> 使能CHACHAPOLY AEAD算法: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_CRYPTO_CHACHAPOLY: 1
```
> 使能X.509证书支持: 默认使能，如需关闭，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_X509: 0
```
> 使能TLS协议支持: 默认使能，如需关闭，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_TLS: 0
```
> 使能TLS1.2协议支持: 默认使能，如需关闭，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_TLS1_2: 0
```
> 使能DTLS协议支持: 默认使能，如需关闭，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_DTLS: 0
```
> 使能TLS协议调试支持: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_TLS_DEBUG: 1
```
> TLS/DTLS最大收发报文长度: 单位：字节，默认为4096。可修改配置如2K：
```yaml
def_config:
  MBEDTLS_CONFIG_TLS_MAX_CONTENT_LEN: 2048
```
> 是否包含自测程序: 默认不包含，如需包含，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_SELFTEST: 1
```
> 使能错误字符串支持: 默认不使能，如需使能，可修改配置如：
```yaml
def_config:
  MBEDTLS_CONFIG_ERROR: 1
```

# API说明
mbedtls API用法与开源版本一致，API用法可以参考[mbedtls官网API说明](https://tls.mbed.org/api/)。

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
  - mbedtls: master          # helloworld_demo中引入mbedtls组件

```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install mbedtls

```

上述命令执行成功后，组件源码则被下载到了./components/mbedtls路径中。

## 步骤4 添加示例

在mbedtls组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/mbedtls/tree/master/example)：

```yaml
source_file:
  - example/tls_example.c #添加编译tls_example.c
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例

**CLI命令行输入：**
```shell

netmgr_example # 执行netmgr示例，使能netmgr命令

```

> 关键日志：
```shell

netmgr test
add_hdl_info:64
add_hdl_info:70

```

**CLI命令行输入：**
```shell

netmgr -t wifi -c <ssid(你的wifi名)> <passworld(你的wifi密码)> # 连接wifi网络

```

> 关键日志：
```shell

wifi event cb
Got IP

```

**CLI命令行输入：**
```shell

tls_example # 连接wifi网络

```

> 关键日志：
```shell

tls_example test success!

```

# 注意事项
无

# FAQ

Q1： TLS 握手失败，Debug消息显示"buffer too small ..." 或者 "bad message length"

答：这通常是由于内部用来处理TLS消息的I/O buffer小于消息的长度，通过修改MBEDTLS_CONFIG_TLS_MAX_CONTENT_LEN配置增大I/O buffer即可。
AliOS Things默认配置的I/O buffer为4KB， 一般扩大到16384 （16KB）可以解决所有的此类问题。

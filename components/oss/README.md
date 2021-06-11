@page oss oss

[更正文档](https://gitee.com/alios-things/oss/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)

# 概述
OSS也叫对象存储服务（Object Storage Service），是阿里云提供的一种存储服务，随着智能设备越来越普及，越来越多终端的内容需要存储在云端，例如录音留言，照片、视频、文件日志等等。下面介绍几个OSS中的概念，后面在开发的过程中会用到

- **存储空间（Bucket）**
存储空间是您用于存储对象（Object）的容器，所有的对象都必须隶属于某个存储空间。存储空间具有各种配置属性，包括地域、访问权限、存储类型等。您可以根据实际需求，创建不同类型的存储空间来存储不同的数据。
- **对象（Object）**
对象是OSS存储数据的基本单元，也被称为OSS的文件。对象由元信息（Object Meta）、用户数据（Data）和文件名（Key）组成。对象由存储空间内部唯一的Key来标识。对象元信息是一组键值对，表示了对象的一些属性，例如最后修改时间、大小等信息，同时您也可以在元信息中存储一些自定义的信息。
- **访问域名（Endpoint）**
Endpoint表示OSS对外服务的访问域名。OSS以HTTP RESTful API的形式对外提供服务，当访问不同地域的时候，需要不同的域名。通过内网和外网访问同一个地域所需要的域名也是不同的。
- **访问密钥（AccessKey）**
AccessKey简称AK，指的是访问身份验证中用到的AccessKey Id和AccessKey Secret。OSS通过使用AccessKey Id和AccessKey Secret对称加密的方法来验证某个请求的发送者身份。AccessKey Id用于标识用户；AccessKey Secret是用户用于加密签名字符串和OSS用来验证签名字符串的密钥，必须保密。
## 版权信息


> Apache license v2.0



## 目录结构


```tree
├── src
│   ├── auth/*.cc               # 证书认证相关实现函数
│   ├── client/*.cc             # 客户端设备核心逻辑实现函数
│   ├── encryption/*.cc         # 加密安全相关部分
│   ├── external                  
│   │   ├── json/*.cc           # json相关的函数
│   │   └── tinyxml2/*.cc       # tinyxml2网络资源
│   ├── http/*.cc               # http相关的网络函数
│   ├── model/*.cc              # OSS模型相关的网络函数
│   ├── utils/*.cc              # OSS组件资源通用部分
│   ├── resumable/*.cc          # 相关恢复逻辑
│   └── *.cc                    # 其他部分
├── include/alibabacloud/oss
│   ├── auth/*.h                # 证书认证相关头文件
│   ├── client/*.h              # 客户端设备核心逻辑头文件
│   ├── encryption/*.h          # 加密安全相关头文件
│   ├── http/*.h                # http相关的函数头文件
│   ├── model/*.h               # OSS模型相关的网络函数头文件
│   ├── utils/*.h               # OSS组件资源通用部分头文件
│   └── *.h                     # 其他头文件
├── package.yaml                # 编译配置文件
└── example
    └── oss_example.c           # OSS测试代码
```


## 依赖组件


- haas100
- mbedtls
- cplusplus
- fatfs



# 常用配置


系统中相关配置已有默认值，如需修改配置，统一在package.yaml修改，具体如下：


> 选择使用CRYPTO MBEDTLS加密方式



```yaml
USE_CRYPTO_MBEDTLS: 1
```


> 选择使用POSIX API的timer接口



```yaml
USE_AOS_TIME_POSIX_API: 1
```


> 选择支持读取SD上传文件，READ_SD_SIZE_MAX可以配置最大读取的文件大小，默认15KB

```yaml
USE_SD_FOR_OSS: 1
READ_SD_SIZE_MAX: 1024*15
```
# API说明

> 在使用OSS组件之前，开发者需要从阿里云平台购买OSS服务获取，在oss_example.c中替换即可。
```c
char AccessKey_ID[] = "Access Key ID";
char AccessKey_Secret[] = "Access Key Secret";
char Endpoint[] = "End point";
char Bucket[] = "Bucket";
```

> OSS上传本地文件的API接口

```c
--------------------------------------------------
--     keyId --> 身份验证AccessKey Id            --
--     keySecret --> 身份验证AccessKey Secret    --
--     endPoint --> OSS对外服务的访问域名          --
--     bucketName --> 存储空间                   --
--     localfilepath --> 本地文件路径             --
--------------------------------------------------
char* oss_upload_local_file(char *keyId, 
                            char *keySecret, 
                            char *endPoint, 
                            char *bucketName, 
                            char* localfilepath);
```
> OSS上传本地字符内容API接口

```c
--------------------------------------------------
--     keyId --> 身份验证AccessKey Id            --
--     keySecret --> 身份验证AccessKey Secret    --
--     endPoint --> OSS对外服务的访问域名          --
--     bucketName --> 存储空间                   --
--     scontent --> 本地字符内容                  --
--------------------------------------------------
char* oss_upload_local_content(char *keyId, 
                               char *keySecret, 
                               char *endPoint, 
                               char *bucketName, 
                               char *scontent);
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
  - linksdk: master
  - mbedtls: master
  - netmgr: master
  - oss: master
```

## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install oss

```

上述命令执行成功后，组件源码则被下载到了./components/oss路径中。

## 步骤4 添加示例

> oss组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/oss/tree/master/example)：

```yaml
source_file:
  - example/oss_example.c
```

> helloworld.c中添加网络相关部分，因为oss依赖网络的连接

```c
#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>

//添加网络
#include "netmgr.h"
#include <uservice/uservice.h>
#include <uservice/eventid.h>

int application_start(int argc, char *argv[])
{
    int count = 0;

    printf("nano entry here!\r\n");
    aos_set_log_level(AOS_LL_DEBUG);
    event_service_init(NULL);
    netmgr_service_init(NULL); //添加网络

    while(1) {
        aos_msleep(1000);
    };
}
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
网络连接：
```sh
netmgr -t wifi -c SSID PWD
```
CLI关键日志：
```sh
[  41.977]<I>WIFI_SERVICE set sntp server:1.cn.pool.ntp.org successfully
[  41.977]<I>WIFI_SERVICE sntp config servadd end.
[  42.231]<I>WIFI_SERVICE [sntp] OK: sec 1616747078 usec 875314
```
**CLI命令行输入：**
测试OSS上传本地字符内容
```sh
oss_test 0 hello_oss  # 上传本地内容，例如hello_oss到oss系统中
```


> CLI关键日志：



```sh
oss upload begin 0...
objectfile_path:oss/test/oss.txt
----- time 1616747084 ---------
read
ProgressCallback[0] => 9 ,9,9
read
ProgressCallback[0] => 0 ,9,9
[makeRequest]response_code: 200
oss ->url:http://xxxxxxxxxx.xxxxxx
oss_upload_local_content success, ETag:ED9BE43B38DDC1254170BEBCA4046368
oss upload end !!!
```
**CLI命令行输入：**
测试OSS上传本地文件
```sh
oss_test 1 /sdcard/oss.txt  # 上传本地文件，例如SD卡读取oss.txt上传
```


> CLI关键日志：



```sh
oss upload begin 1...
objectfile_path:sdcard/oss.txt
localfile_path:/sdcard/oss.txt
[ 106.852]<A>AOS open file /sdcard/oss.txt
[ 106.854]<A>AOS SD open file size <51>.
----- time 1616747022 ---------
oss ->url:http://xxxxxxxxxx.xxxxxxxxxx
oss_upload_local_file success, ETag:22065065146EAA25CFB9FF5E774DD1C9
oss upload end !!!
```
# FAQ


Q1： OSS本组件依赖网络吗？


答：是的，OSS组件只有在网络正常的时候才可以使用。

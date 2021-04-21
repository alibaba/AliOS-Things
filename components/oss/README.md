@page oss oss

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


```
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



```
USE_CRYPTO_MBEDTLS: 1
```


> 选择使用POSIX API的timer接口



```
USE_AOS_TIME_POSIX_API: 1
```


> 选择支持读取SD上传文件，READ_SD_SIZE_MAX可以配置最大读取的文件大小，默认15KB

```
USE_SD_FOR_OSS: 1
READ_SD_SIZE_MAX: 1024*15
```
# API说明

> 在使用OSS组件之前，开发者需要从阿里云平台购买OSS服务获取，在oss_example.c中替换即可。
```
char AccessKey_ID[] = "Access Key ID";
char AccessKey_Secret[] = "Access Key Secret";
char Endpoint[] = "End point";
char Bucket[] = "Bucket";
```

> OSS上传本地文件的API接口

```
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

```
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


示例代码参考example/oss_example.c，以运行helloworld_demo为例，具体步骤如下：


## 添加示例代码


> oss组件的package.yaml中添example示例代码



```
source_file:
  - example/oss_example.c
```
## helloworld_demo中添加oss组件


> helloworld_demo的package.yaml中添加

```
depends:
  - linksdk: dev_aos
  - mbedtls: dev_aos
  - netmgr: dev_aos
  - oss: dev_aos
```


> helloworld.c中添加网络相关部分，因为oss依赖网络的连接

```
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
## 编译


```
cd solutions/helloworld_demo && aos make
```
## 烧录固件


> 参考具体板子的快速开始文档。

## OSS示例测试
### CLI命令行输入：
网络连接：
```
netmgr -t wifi -c SSID PWD
```
CLI关键日志：
```
[  41.977]<I>WIFI_SERVICE set sntp server:1.cn.pool.ntp.org successfully
[  41.977]<I>WIFI_SERVICE sntp config servadd end.
[  42.231]<I>WIFI_SERVICE [sntp] OK: sec 1616747078 usec 875314
```
### CLI命令行输入：
测试OSS上传本地字符内容
```
oss_test 0 hello_oss  # 上传本地内容，例如hello_oss到oss系统中
```


> CLI关键日志：



```
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
### CLI命令行输入：
测试OSS上传本地文件
```
oss_test 1 /sdcard/oss.txt  # 上传本地文件，例如SD卡读取oss.txt上传
```


> CLI关键日志：



```
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

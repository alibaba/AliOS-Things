@page curl curl

[更正文档](https://gitee.com/alios-things/curl/edit/master/README.md) &emsp;&emsp;&emsp;&emsp; [贡献说明](https://help.aliyun.com/document_detail/302301.html)

# 概述
cURL是一个开源项目，主要产品是curl（命令行工具）和libcurl（C语言的API库），两者功能均是：基于网络协议，对指定URL进行网络传输.cURL是关注于网络协议的传输，不关注具体数据的处理。

cURL支持的通信协议有DICT, FILE, FTP, FTPS, GOPHER, HTTP, HTTPS, IMAP, IMAPS, LDAP, LDAPS, POP3, POP3S, RTMP, RTMPS, RTSP, SCP, SFTP, SMB, SBMS, SMTP, SMTPS, TELNET 和TFTP
-
## 版权信息
> [Copyright - License](https://curl.se/docs/copyright.html)

## 目录结构
```tree
.
├── example                     # 示例程序
│   ├── urlapi.c
├── include
│   ├── curl                 # 对外头文件
├── package.yaml                # 编译文件
├── README.md                   # 说明文档
├── lib                   # api库
└── src                   # 命令行工具

```

## 依赖组件
* osal_aos
* lwip
* netmgr

# 常用配置
无
# API说明
[curl API](https://curl.se/libcurl/c/)

# 使用示例
组件使用示例相关的代码下载、编译和固件烧录均依赖AliOS Things配套的开发工具，所以首先需要参考[《AliOS Things集成开发环境使用说明之搭建开发环境》](https://help.aliyun.com/document_detail/302378.html)，下载安装。
待开发环境搭建完成后，可以按照以下步骤进行示例的测试。

## 步骤1 创建或打开工程

**打开已有工程**

如果用于测试的案例工程已存在，可参考[《AliOS Things集成开发环境使用说明之打开工程》](https://help.aliyun.com/document_detail/302381.html)打开已有工程。

**创建新的工程**

组件的示例代码可以通过编译链接到AliOS Things的任意案例（solution）来运行，这里选择helloworld_demo案例。helloworld_demo案例相关的源代码下载可参考[《AliOS Things集成开发环境使用说明之创建工程》](https://help.aliyun.com/document_detail/302379.html)。

## 步骤2 添加组件

案例下载完成后，需要在helloworld_demo组件的package.yaml中添加对curl组件的依赖：
> helloworld_demo组件的package.yaml中添加
```yaml
depends:
  - curl: master # helloworld_demo中引入curl组件
```

## 步骤3 下载组件

在已安装了  的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install curl

```

上述命令执行成功后，组件源码则被下载到了./components/curl

## 步骤4 添加示例

在curl组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/curl/tree/master/example)：

```yaml
source_file:
  - "src/*.c"
  - "example/*.c" # add example source
```

在helloworld_demo的application_start函数中添加网络服务初始化代码：

```c
int application_start(int argc, char *argv[])
{
    aos_set_log_level(AOS_LL_DEBUG);
    event_service_init(NULL);

    netmgr_service_init(NULL);

    while (1) {
        aos_msleep(1000);
    };

    return 0;
}
```
## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《AliOS Things集成开发环境使用说明之编译固件》](https://help.aliyun.com/document_detail/302384.html)。

## 步骤6 烧录固件

helloworld_demo案例的固件生成后，可参考[《AliOS Things集成开发环境使用说明之烧录固件》](https://help.aliyun.com/document_detail/302383.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《AliOS Things集成开发环境使用说明之查看日志》](https://help.aliyun.com/document_detail/302382.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。

## 步骤8 测试示例
> 烧入固件后，输入cli命令：

```shell
curl_urlapi_example
```

可看到如下打印：
```shell
<body>
<div>
    <h1>Example Domain</h1>
    <p>This domain is for use in illustrative examples in documents. You may use this
    domain in literature without prior coordination or asking for permission.</p>
    <p><a href="https://www.iana.org/domains/example">More information...</a></p>
</div>
</body>
</html>
* STATE: PERFORM => DONE handle 0x34046218; line 1909 (connection #0)
* multi_done
* Connection #0 to host example.com left intact
* Expire cleared (transfer 0x34046218)
```

# 注意事项
无

# FAQ
无

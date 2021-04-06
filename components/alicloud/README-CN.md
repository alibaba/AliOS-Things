[English](./README.md) | 简体中文

<p align="center">
<a href="https://www.alibabacloud.com"><img src="https://aliyunsdk-pages.alicdn.com/icons/Aliyun.svg"></a>
</p>

<h1 align="center">Alibaba Cloud SDK for C++</h1>

<p align="center">
<a href="https://travis-ci.org/aliyun/aliyun-openapi-cpp-sdk"><img src="https://travis-ci.org/aliyun/aliyun-openapi-cpp-sdk.svg?branch=master" alt="Travis Build Status"></a>
<a href="https://codecov.io/gh/aliyun/aliyun-openapi-cpp-sdk"><img src="https://codecov.io/gh/aliyun/aliyun-openapi-cpp-sdk/branch/master/graph/badge.svg" alt="codecov"></a>
</p>

Alibaba Cloud SDK for C++ 让您不用复杂编程即可访问云服务器、负载均衡、云监控等阿里云服务。这里向您介绍如何获取 Alibaba Cloud SDK for C++ 并开始调用。

如果您在使用的过程中遇到任何问题，欢迎前往[阿里云SDK问答社区](https://yq.aliyun.com/tags/type_ask-tagid_23350)提问，提问前请阅读[提问引导](https://help.aliyun.com/document_detail/93957.html)。亦可在当前 GitHub [提交 Issues](https://github.com/aliyun/aliyun-openapi-cpp-sdk/issues/new)。

## 使用诊断
[Troubleshoot](https://troubleshoot.api.aliyun.com/?source=github_sdk) 提供 OpenAPI 使用诊断服务，通过 `RequestID` 或 `报错信息` ，帮助开发者快速定位，为开发者提供解决方案。

## 环境要求

在使用 Alibaba Cloud SDK for C++ 前，确保您已经：

* 注册了阿里云账号并获取了访问密钥（AccessKey）。

> **说明：** 为了保证您的账号安全，建议您使用RAM账号来访问阿里云服务。阿里云账号对拥有的资源有全部权限。RAM账号由阿里云账号授权创建，仅有对特定资源限定的操作权限。详情[参见RAM](https://help.aliyun.com/document_detail/28647.html)。

* 开通了云产品服务。有些云产品如对象存储（OSS）需要先在[阿里云控制台](https://home.console.aliyun.com)开通服务。

* 安装支持 C++ 11 或更高版本的编译器：
  * Windows: Visual Studio 2015 或以上版本
  * Linux: GCC 4.9 或以上版本

* 安装 CMake 3.0 或以上版本
* 建议 4G 或以上内存

## 安装 (从源代码构建)

### Linux

1. 要在 Linux 平台进行编译, 您必须安装依赖的外部库文件 libcurl、libopenssl、libuuid、libjsoncpp, 通常情况下，系统的包管理器中的会有提供。

* 例如：在基于 Redhat / Fedora 的系统上安装这些软件包

```bash
# use yum
yum install jsoncpp-devel openssl-devel uuid-devel libcurl-devel

# use dnf
sudo dnf install libcurl-devel openssl-devel libuuid-devel libjsoncpp-devel
```

* 在基于 Debian/Ubuntu 的系统

```bash
sudo apt-get install libcurl4-openssl-dev libssl-dev uuid-dev libjsoncpp-dev
```

2. 执行以下命令，从 Github 克隆源码

```bash
git clone https://github.com/aliyun/aliyun-openapi-cpp-sdk.git
```

3. 编译安装

* 手动编译安装

```bash
cd aliyun-openapi-cpp-sdk
mkdir sdk_build
cd sdk_build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install
```

* 或者通过 `easyinstall.sh`一键式安装

```bash
cd aliyun-openapi-cpp-sdk
sudo sh easyinstall.sh <lower name of product (like 'ecs')>

# example
sudo sh easyinstall.sh core
sudo sh easyinstall.sh ecs
```

**Alibaba Cloud SDK for C++ 将被安装在 `/usr` .**

### Windows

1. 安装 [perl](https://www.perl.org/get.html#win32)

2. 使用 git-bash 执行以下命令，从 Github 克隆源码

  ```bash
  git clone https://github.com/aliyun/aliyun-openapi-cpp-sdk.git
  ```

3. 使用 Visual Studio 进行编译:

* 在 aliyun-openapi-cpp-sdk 下创建 `sdk_build` 目录
* 打开 cmake-gui , 然后进行以下操作
  * 选择 `Browse Source` 为 `源代码目录(aliyun-openapi-cpp-sdk)`
  * 选择 `Browse build` 为 `构建目录(sdk_build)`
  * 点击 `configure`
  * 使用小写的产品名称，设置 `BUILD_PRODUCT` 参数的值
  * 点击 `generate`, 构建 VS 解决方案。

4. 编译安装 C++ SDK

* 进入 sdk_build 目录，使用 Visual Studio 打开 alibabacloud-sdk.sln 解决方案
* 选择构建 `Release` 输出
* 并打开配置管理器勾选 `INSTALL`
* 构建 -> 生成解决方案

**Alibaba Cloud SDK for C++ 将安装在 `C:\Program File (x86)\alibabacloud-sdk` 目录**

**注意：请以管理员身份运行 Visual Studio 和 cmake-gui，否则无法安装 SDK**

---

## 快速使用

在调用 Alibaba Cloud SDK for C++ 时，您首先需要配置预处理器定义 ALIBABACLOUD_SHARED 以引用阿里云 C++ SDK 的共享库，然后通过创建 Client 实例提供身份验证，并指定云服务的地域，然后发送API请求。

以下代码展示了如何调用 DescribeInstancesAPI 查询指定地域所有 ECS 实例的详细信息。

> **说明：** 您需要替换示例中的 your-region-id、your-access-key-id 和 your-access-key-secret 的值。

```cpp
#include <iostream>
#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/ecs/EcsClient.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Ecs;

int main(int argc, char** argv) {
  // 初始化 SDK
  AlibabaCloud::InitializeSdk();

  // 配置 ecs 实例
  ClientConfiguration configuration("<your-region-id>");
  EcsClient client("<your-access-key-id>", "<your-access-key-secret>", configuration);

  // 创建API请求并设置参数
  Model::DescribeInstancesRequest request;
  request.setPageSize(10);

  auto outcome = client.describeInstances(request);
  if (!outcome.isSuccess()) {
    // 异常处理
    std::cout << outcome.error().errorCode() << std::endl;
    AlibabaCloud::ShutdownSdk();
    return -1;
  }

  std::cout << "totalCount: " << outcome.result().getTotalCount() << std::endl;

  // 关闭 SDK
  AlibabaCloud::ShutdownSdk();
  return 0;
}
```

复制上述文件到 ecs_test.cc。

Linux 下

```bash
~$ g++ -o ecstest ecs_test.cc --std=c++11 -lalibabacloud-sdk-core -l alibabacloud-sdk-ecs
~$ ./ecstest
# 结果或错误返回将在此展示
~$
```

## Timeout 设置

CPP SDK 使用 libcurl 作为底层 HTTP 传输库。

* 下面两个参数用来传递超时参数到 libcurl。
  * `connectTimeout`: 连接超时设置。 [参考](https://curl.haxx.se/libcurl/c/CURLOPT_CONNECTTIMEOUT_MS.html).
  * `readTimeout`: 传输超时设置。[参考](https://curl.haxx.se/libcurl/c/CURLOPT_TIMEOUT_MS.html)

* 默认值
  * connectTimeout: 5000ms
  * readTimeout: 10000ms

* 可以在创建 Client 或者发 Requst 设置超时参数。

* Requst 设置优先级高于 Client 设置。

* 输入 0 或者 -1 到 `setConnectTimeout` 和 `setReadTimeout` 可以禁用此功能。

下面代码是设置超时参数的例子，由于 Request 优先级高于 Client，所以最终 `ConnectTimeout` 为 `1000ms`， `readTimeout` 为 `6000ms`。

```cpp
#include <iostream>
#include <alibabacloud/core/AlibabaCloud.h>
#include <alibabacloud/ecs/EcsClient.h>

using namespace AlibabaCloud;
using namespace AlibabaCloud::Ecs;

int main(int argc, char** argv) {
  // Initialize the SDK
  AlibabaCloud::InitializeSdk();

  // Configure the ECS instance
  ClientConfiguration configuration("<your-region-id>");
  // specify timeout when create client.
  configuration.setConnectTimeout(1500);
  configuration.setReadTimeout(4000);

  EcsClient client("<your-access-key-id>", "<your-access-key-secret>", configuration);

  // Create an API request and set parameters
  Model::DescribeInstancesRequest request;
  request.setPageSize(10);
  // specify timeout when request
  request.setConnectTimeout(1000);
  request.setReadTimeout(6000);

  auto outcome = client.describeInstances(request);
  if (!outcome.isSuccess()) {
    // Handle exceptions
    std::cout << outcome.error().errorCode() << std::endl;
    AlibabaCloud::ShutdownSdk();
    return -1;
  }

  std::cout << "totalCount: " << outcome.result().getTotalCount() << std::endl;

  // Close the SDK
  AlibabaCloud::ShutdownSdk();
  return 0;
}

```

---

**更多 [例程](https://github.com/aliyun/aliyun-openapi-cpp-sdk/tree/master/examples)**

**参考ucloud_ai组件调用alicloud AI API接口及example用法**

## 问题

[提交 Issue](https://github.com/aliyun/aliyun-openapi-cpp-sdk/issues/new/choose), 不符合指南的问题可能会立即关闭。

## 发行说明

每个版本的详细更改记录在[发行说明](CHANGELOG)中。

## 贡献

提交 Pull Request 之前请阅读[贡献指南](CONTRIBUTING.md)。

## 许可证

[Apache-2.0](http://www.apache.org/licenses/LICENSE-2.0)

版权所有 1999-2019 阿里巴巴集团

English | [简体中文](./README-CN.md)

<p align="center">
<a href="https://www.alibabacloud.com"><img src="https://aliyunsdk-pages.alicdn.com/icons/AlibabaCloud.svg"></a>
</p>

<h1 align="center">Alibaba Cloud SDK for C++</h1>

<p align="center">
<a href="https://travis-ci.org/aliyun/aliyun-openapi-cpp-sdk"><img src="https://travis-ci.org/aliyun/aliyun-openapi-cpp-sdk.svg?branch=master" alt="Travis Build Status"></a>
<a href="https://codecov.io/gh/aliyun/aliyun-openapi-cpp-sdk"><img src="https://codecov.io/gh/aliyun/aliyun-openapi-cpp-sdk/branch/master/graph/badge.svg" alt="codecov"></a>
</p>

Alibaba Cloud SDK for C++ allows you to access Alibaba Cloud services such as Elastic Compute Service (ECS), Server Load Balancer (SLB), and CloudMonitor. You can access Alibaba Cloud services without the need to handle API related tasks, such as signing and constructing your requests.

This document introduces how to obtain and call this SDK.

If you have any problem while using Alibaba Cloud SDK for C++, please submit an [issue](https://github.com/aliyun/aliyun-openapi-cpp-sdk/issues/new).

## Troubleshoot
[Troubleshoot](https://troubleshoot.api.aliyun.com/?source=github_sdk) Provide OpenAPI diagnosis service to help developers locate quickly and provide solutions for developers through `RequestID` or `error message`.

## Requirements

- To use this SDK, you must have an Alibaba Cloud account and an AccessKey.

  The AccessKey is required when initializing the client. You can create an AccessKey in the Alibaba Cloud console. For more information, see [Create an AccessKey](https://usercenter.console.aliyun.com/?spm=5176.doc52740.2.3.QKZk8w#/manage/ak).

  >**Note:** To increase the security of your account, we recommend that you use the AccessKey of the RAM user to access Alibaba Cloud services.

- To use this SDK to access the APIs of a product, you must first activate the product on the [Alibaba Cloud console](https://home.console.aliyun.com/?spm=5176.doc52740.2.4.QKZk8w) if required.

- C++11 supported compiler installed
  - Windows: Visual Studio 2015 or newer
  - Linux: GCC 4.9 or newer
- CMake 3.0 or newer
- 4G memory or more

## Installation

### Linux

1. Install third-party libraries on the Linux platform, including `libcurl`, `libopenssl`, `libuuid`, and `libjsoncpp`.

- Run the following commands on the `Redhat/Fedora` system

```bash
# use yum
yum install jsoncpp-devel openssl-devel uuid-devel libcurl-devel

# use dnf
sudo dnf install libcurl-devel openssl-devel libuuid-devel libjsoncpp-devel
```

- Run the following commands on the `Debian/Ubuntu` system

```bash
sudo apt-get install libcurl4-openssl-dev libssl-dev uuid-dev libjsoncpp-dev
```

2. Run the following commands to clone source codes from GitHub.

```bash
git clone https://github.com/aliyun/aliyun-openapi-cpp-sdk.git
```

3. Build and install SDK

- Manually build and install

```bash
cd aliyun-openapi-cpp-sdk
mkdir sdk_build
cd sdk_build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install
```

- Or you may do this with `easyinstall.sh` in the source directory

```bash
cd aliyun-openapi-cpp-sdk
sudo sh easyinstall.sh <lower name of product (like 'ecs')>

# example
sudo sh easyinstall.sh core
sudo sh easyinstall.sh ecs
```

**The Alibaba Cloud SDK for C++ will be installed to `/usr`.**

### Windows

1. Install [perl](https://www.perl.org/get.html#win32).

2. Run the following command to clone code from Github via git-bash:

```bash
git clone https://github.com/aliyun/aliyun-openapi-cpp-sdk.git
```

3. Build Visual Studio solution

- Change directory to source code and make directory sdk_build

- Open CMake UI and

  - `Browse Source` to open source code directory.

  - `Browse build`  to open the created `sdk_build` directory

  - `Configure`

  - use the lower name of product to set the value of `BUILD_PRODUCT`

  - `Generate`

4. Build and Install C++ SDK

- Open `aliyun-openapi-cpp-sdk\\sdk_build\\alibabacloud-sdk.sln` with Visual Studio

- Select  `Release`

- Check INSTALL option from Build -> Configuration Manager

- Build->Build Solutions to build.

**Alibaba Cloud SDK for C++ will be installed to `C:\Program File (x86)\alibabacloud-sdk`**

---

## Quick Examples

Before using this SDK, you must first configure the preprocessor to define `ALIBABACLOUD_SHARED` to achieve dynamic linking with the SDK shared libraries. Then you must create a client instance, specify the region of cloud services and provide authentication parameters before sending API requests.

The following code shows how to call the [DescribeInstances](~~25506~~) API of ECS to query detailed information of all ECS instances in a specific region.

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
  EcsClient client("<your-access-key-id>", "<your-access-key-secret>", configuration);

  // Create an API request and set parameters
  Model::DescribeInstancesRequest request;
  request.setPageSize(10);

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

Copy the above to ecs_test.cc, then build with the following command.

```bash
~$ g++ -o ecstest ecs_test.cc --std=c++11 -lalibabacloud-sdk-core -l alibabacloud-sdk-ecs
~$ ./ecstest
# Result or error message will be shown here.
~$
```

## Timeout Configuration

CPP SDK uses libcurl to do HTTP transfer.

- The following timeout parameters are used to for libcurl.

 - `connectTimeout`: timeout for the connect phase. [Refer](https://curl.haxx.se/libcurl/c/CURLOPT_CONNECTTIMEOUT_MS.html).
 - `readTimeout`: maximum time the request is allowed to take, [Refer](https://curl.haxx.se/libcurl/c/CURLOPT_TIMEOUT_MS.html)

- Default Value
 - `connectTimeout`: 5000ms
 - `readTimeout`: 10000ms

- You may specify `timeout` parameters when create a client or make a request.

- Request timeout has higher priority than client timeout.

- If you want to disable timeout feature, deliver `0` or `-1` to `setConnectTimeout` and `setReadTimeout`.

The following code shows hot to specify `timeout` parameters, and the final connectTimeout is 1000ms and readTimeout 6000ms.

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

**More [examples](https://github.com/aliyun/aliyun-openapi-cpp-sdk/tree/master/examples)**

## Issues
[Opening an Issue](https://github.com/aliyun/aliyun-openapi-cpp-sdk/issues/new/choose), Issues not conforming to the guidelines may be closed immediately.

## Changelog
Detailed changes for each release are documented in the [release notes](CHANGELOG).

## Contribution
Please make sure to read the [Contributing Guide](CONTRIBUTING.md) before making a pull request.

## License
[Apache-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Copyright 1999-2019 Alibaba Group Holding Ltd.

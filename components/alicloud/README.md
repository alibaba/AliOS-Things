@page alicloud alicloud

# 概述
alicloud组件是基于Alibaba Cloud SDK for C++提供的阿里云服务组件，alicloud对SDK进行了裁剪移植，目前主要支持阿里云视觉智能开放平台AI调用服务。后续如有其他阿里云相关云服务，可放于此目录。

在AliOS Things中，ucloud_ai对此组件进行了封装，因此开发者不需要直接使用该组件，而使用ucloud_ai组件中的接口即可。

# 版权信息
> Apache license v2.0

# 目录结构
```sh
├── CHANGELOG
├── CONTRIBUTING.md
├── core                      # 核心网络接口
│   ├── CMakeLists.txt
│   ├── include
│   └── src
├── Doxyfile
├── easyinstall.sh
├── examples                  # 测试用例
│   ├── CMakeLists.txt
│   ├── facebody
│   ├── imagesearch
│   ├── nlp
│   ├── README.md
│   ├── README_zh.md
│   └── utils.h
├── facebody                   # 人体人脸比对相关OpenAPI接口
│   ├── CMakeLists.txt
│   ├── include
│   └── src
├── function_test.sh
├── generate_code_cov.sh
├── imageenhan                 # 图像增强相关OpenAPI接口
│   ├── CMakeLists.txt
│   ├── include
│   └── src
├── imagerecog                 # 图像识别相关OpenAPI接口
│   ├── CMakeLists.txt
│   ├── include
│   └── src
├── imageseg                   # 图像分割相关OpenAPI接口
│   ├── CMakeLists.txt
│   ├── include
│   └── src
├── LICENSE
├── objectdet                  # 目标检测相关OpenAPI接口
│   ├── CMakeLists.txt
│   ├── include
│   └── src
├── ocr                        # 文本识别相关OpenAPI接口
│   ├── CMakeLists.txt
│   ├── include
│   └── src
├── package.yaml
├── README-CN.md
├── README-ENG.md
├── README.md
├── test                       # 测试相关用例
│   ├── core
│   ├── function_test
│   └── httpserver
├── unit_test
```

# 依赖组件
* mbedtls
* jsoncpp

# 常用配置
```sh
def_config:                              # 组件的可选项
  USE_CRYPTO_MBEDTLS: 1
```
# API说明
NA

# 使用示例

## 不建议开发者直接使用该组件，AI能力请使用ucloud_ai组件。

## 添加alicloud组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - alicloud: dev_aos            # helloworld_demo组件中引入alicloud组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档。

## alicloud示例测试
请参考 @ref ucloud_ai 组件的测试用例，位于ucloud_ai/src/ucloud_ai_example.c中。

# FAQ
NA


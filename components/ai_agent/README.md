@page ai_agent ai_agent

# 概述
AI Agent作为AI的代理引擎框架，支持不同推理引擎的注册，对用户提供统一的接口，简化用户操作，目前注册的AI引擎为ucloud_ai云端推理引擎，用户可以根据需要注册其他推理引擎。

## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── include
│   ├── aiagent_common.h         # aiagent模型定义头文件
│   ├── aiagent_engine.h         # aiagent引擎注册结构体定义头文件
│   ├── aiagent_service.h        # aiagent对外服务接口定义头文件
│   ├── engine
│   │   └── ucloud_ai_engine.h   # ucloud ai引擎头文件
│   └── aiconfig.h               # AI引擎配置文件
├── package.yaml                 # 编译配置文件
└── src
    ├── aiagent_engine.c         # aiagent引擎管理代码
    ├── aiagent_service.c        # aiagent服务接口实现
    └── engine
    │   └── ucloud_ai_engine.c   # ucloud ai引擎注册实例
    └── example
        └── aiagent_example.c
```

## 依赖组件
* ai_agent

# 常用配置
```sh
def_config:                      # 组件的可配置项
  CONFIG_UCLOUD_AI_ENGINE_ENABLE: 1
```
> 配置AI引擎为ucloud ai推理引擎

# API说明
@ref aiagent_aos_api

# 使用示例
示例代码参考example/aiagent_example.c，以运行ucloud_ai_demo为例，具体步骤如下：

## 添加示例代码
> ai_agent组件的package.yaml中添加example
```sh
source_file:
  - "example/aiagent_example.c" # add aiagent_example.c
```

> helloworld_demo组件的application_start中添加代码
```sh
    /*init network service*/
    netmgr_service_init(NULL);
```

## 云端功能开通
> 登陆https://vision.aliyun.com开通如下功能：
```sh
人脸人体: https://vision.aliyun.com/facebody
文字识别: https://vision.aliyun.com/ocr
分割抠图: https://vision.aliyun.com/imageseg
目标检测: https://vision.aliyun.com/objectdet

如没有阿里云账号，请登陆http://www.aliyun.com开通
```

> 登陆oss.console.aliyun.com创建bucket
```sh
1. 创建Bucket时地域选择“上海”
2. 读写权限选择“公共读”
```
## aiconfig.h中配置OSS信息
```sh
OSS_ACCESS_KEY "<Your-Access-Key>"
OSS_ACCESS_SECRET "<Your-Access-Secret>"
OSS_ENDPOINT "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET "Your-OSS-Bucket"

KEY以及SECRET获取链接: https://usercenter.console.aliyun.com/#/accesskey
ENDPOINT使用默认即可，BUCKET请使用你创建好的Bucket名称
```

## 添加ai_agent组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - ai_agent: dev_aos # helloworld_demo中引入ai_agent组件
  - netmgr: dev_aos # helloworld_demo中引入netmgr组件
```

## 编译
```sh
cd solutions/ucloud_ai_demo && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档。

    此外，本组件例子中使用到到图片存放在代码中hardware/chip/haas1000/prebuild/data目录，除烧录helloworld demo image外，需额外使用如下命令烧录littlefs文件系统:
```sh
aos burn -f hardware/chip/haas1000/release/write_flash_tool/ota_bin/littlefs.bin#0xB32000
```

## ucloud_ai示例测试

### CLI命令行输入：
```sh
aiagent -m 0  # 人脸比对
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 1  # 人物动漫化
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 2  # 表情识别
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 3  # 目标检测
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 4  # 主体检测
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 5  # 通用分割
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 6  # 面部分割
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 7  # 身份证证明识别
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 8  # 身份证反面识别
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 9  # 银行卡识别
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 10  # 文本识别
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 11  # 垃圾分类
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 12  # 水果检测
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 13  # 图像人体擦除
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
aiagent -m 14  # 风格迁移
```

> CLI关键日志：
```sh
TODO
```

# 注意事项
NA

# FAQ
NA


@page ucloud_ai ucloud_ai

# 概述
ucloud ai是基于alicloud OpenAPI实现的云端AI推理引擎，对接的阿里云视觉智能开放平台AI能力，目前支持15种AI模型：人脸识别、表情识别、人物动漫化、物体检测、主体检测、通用分割、人脸分割、身份证识别、银行卡识别、文字识别、垃圾分类、水果识别、图像人体擦除、风格迁移，未来将加入更多AI能力。

## 版权信息
> Apache license v2.0

## 目录结构
```sh
├── include
│   ├── model
│   │   ├── common.h           #model通用函数内部头文件
│   │   ├── facebody.h         #云端人脸人体识别函数内部头文件
│   │   ├── imageenhan.h       #云端图像增强处理函数内部头文件
│   │   ├── imagerecog.h       #云端图像识别处理函数内部头文件
│   │   ├── imageseg.h         #云端图像分割处理函数内部头文件
│   │   ├── internal.h         #云端节点配置内部头文件
│   │   ├── objectdet.h        #云端目标检测处理函数内部头文件
│   │   └── ocr.h              #云端OCR识别处理函数内部头文件
│   ├── ucloud_ai_common.h     #云端AI模型配置对外头文件
│   ├── ucloud_ai_facebody.h   #云端人脸人体识别函数接口对外头文件
│   ├── ucloud_ai_imageenhan.h #云端图像增强处理函数接口对外头文件
│   ├── ucloud_ai_imagerecog.h #云端图像识别处理函数接口对外头文件
│   ├── ucloud_ai_imageseg.h   #云端图像分割处理函数接口对外头文件
│   ├── ucloud_ai_objectdet.h  #云端目标检测处理函数接口对外头文件
│   └── ucloud_ai_ocr.h        #云端OCR识别处理函数接口对外头头文件
├── package.yaml               # 编译配置文件
└── src
    ├── model
    │   ├── common.cc          #model通用函数代码
    │   ├── facebody.cc        #云端人脸人体识别函数接口代码
    │   ├── imageenhan.cc      #云端图像增强函数接口代码
    │   ├── imagerecog.cc      #云端图像识别函数接口代码
    │   ├── imageseg.cc        #云端图像分割函数接口代码
    │   ├── objectdet.cc       #云端目标检测函数接口代码
    │   └── ocr.cc             #云端OCR识别函数接口代码
    ├── ucloud_ai_common.c     #云端文件上传及配置函数接口代码
    ├── ucloud_ai_facebody.c   #云端人脸识别对外函数接口代码
    ├── ucloud_ai_imageenhan.c #云端图像增强对外函数接口代码
    ├── ucloud_ai_imagerecog.c #云端图像识别对外函数接口代码
    ├── ucloud_ai_imageseg.c   #云端图像分割对外函数接口代码
    ├── ucloud_ai_objectdet.c  #云端目标检测对外函数接口代码
    └── ucloud_ai_ocr.c        #云端OCR识别对外函数接口代码
```

## 依赖组件
* alicloud
* http

# 常用配置
```sh
def_config:                              # 组件的可配置项
  CONFIG_ALICLOUD_FACEBODY_ENABLE: 1
  CONFIG_ALICLOUD_IMAGERECOG_ENABLE: 1
  CONFIG_ALICLOUD_IMAGEENHAN_ENABLE: 1
  CONFIG_ALICLOUD_OBJECTDET_ENABLE: 1
  CONFIG_ALICLOUD_IMAGESEG_ENABLE: 1
  CONFIG_ALICLOUD_OCR_ENABLE: 1
  CONFIG_ALICLOUD_OSS_ENABLE: 1
```
> 配置支持的云端AI模型

# API说明
@ref ucloud_ai_aos_api

# 使用示例
示例代码参考example/ucloud_ai_example.c，以运行ucloud_ai_demo为例，具体步骤如下：

## 添加示例代码
> ucloud_ai组件的package.yaml中添加example
```sh
source_file:
  - "example/ucloud_ai_example.c" # add ucloud_ai_example.c
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
## ucloud_ai_example.c中配置OSS信息
```sh
OSS_ACCESS_KEY "<Your-Access-Key>"
OSS_ACCESS_SECRET "<Your-Access-Secret>"
OSS_ENDPOINT "oss-cn-shanghai.aliyuncs.com"
OSS_BUCKET "Your-OSS-Bucket"

KEY以及SECRET获取链接: https://usercenter.console.aliyun.com/#/accesskey
ENDPOINT使用默认即可，BUCKET请使用你创建好的Bucket名称

```

## 添加ucloud_ai组件
> helloworld_demo组件的package.yaml中添加
```sh
depends:
  - ucloud_ai: dev_aos # helloworld_demo中引入ucloud_ai组件
  - netmgr: dev_aos     # helloworld_demo中引入netmgr组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```

## 烧录固件
> 参考具体板子的快速开始文档。

    本组件例子中使用到到图片存放在代码中hardware/chip/haas1000/prebuild/data目录，除烧录helloworld demo image外，需额外使用如下命令烧录littlefs文件系统:
```sh
aos burn -f hardware/chip/haas1000/release/write_flash_tool/ota_bin/littlefs.bin#0xB32000
```

## ucloud_ai示例测试

### CLI命令行输入：
```sh
ucloud_ai -m 0  # 人脸比对
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 1  # 人物动漫化
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 2  # 表情识别
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 3  # 目标检测
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 4  # 主体检测
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 5  # 通用分割
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 6  # 面部分割
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 7  # 身份证证明识别
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 8  # 身份证反面识别
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 9  # 银行卡识别
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 10  # 文本识别
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 11  # 垃圾分类
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 12  # 水果检测
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 13  # 图像人体擦除
```

> CLI关键日志：
```sh
TODO
```

### CLI命令行输入：
```sh
ucloud_ai -m 14  # 风格迁移
```

> CLI关键日志：
```sh
TODO
```

# 注意事项
NA

# FAQ
NA


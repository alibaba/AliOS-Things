@page ai_agent ai_agent

**[更正文档](https://gitee.com/alios-things/ai_agent/edit/master/README.md)** &emsp;&emsp;&emsp;&emsp; **[贡献说明](https://g.alicdn.com/alios-things-3.3/doc/contribute_doc.html)**

# 概述
AI Agent作为AI的代理引擎框架，支持不同推理引擎的注册，对用户提供统一的接口，简化用户操作，目前注册的AI引擎为ucloud-ai云端推理引擎，用户可以根据需要注册其他推理引擎。

## 版权信息
> Apache license v2.0

## 目录结构
```tree
├── include
│   ├── aiagent_common.h         # aiagent模型定义头文件
│   ├── aiagent_engine.h         # aiagent引擎注册结构体定义头文件
│   ├── aiagent_service.h        # aiagent对外服务接口定义头文件
│   ├── engine
│   │   └── ucloud_ai_engine.h   # ucloud ai引擎头文件
│   └── aiconfig.h               # AI引擎配置文件
├── package.yaml                 # 编译配置文件
├── src
│   ├── aiagent_engine.c         # aiagent引擎管理代码
│   ├── aiagent_service.c        # aiagent服务接口实现
│   └── engine
│   │   └── ucloud_ai_engine.c   # ucloud ai引擎注册实例
└── example
    └── aiagent_example.c        # 测试用例
```

## 依赖组件

* ucloud_ai

# 常用配置
```yaml
def_config:                      # 组件的可配置项
  CONFIG_UCLOUD_AI_ENGINE_ENABLE: 1
```
> 配置AI引擎为ucloud ai推理引擎

# API说明
- 参考 [aiagent_aos_api](https://g.alicdn.com/alios-things-3.3/doc/group__aiagent__aos__api.html)

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
  - ai_agent: master   # helloworld_demo中引入ai_agent组件
  - netmgr: master     # helloworld_demo中引入netmgr组件
  - littlefs: master   # helloworld_demo中引入littlefs组件

def_config:
    CLI_IOBOX_ENABLE: 1        # 可选：使能文件系统测试命令，例如ls, ll, mkdir, touch, echo等

```


## 步骤3 下载组件

在已安装了 **alios-studio** 的开发环境工具栏中，选择Terminal -> New Terminal启动终端，并且默认工作路径为当前工程的workspace，此时在终端命令行中输入：

```shell

aos install ai_agent

```

上述命令执行成功后，组件源码则被下载到了./components/ai_agent路径中。

## 步骤4 添加示例

在ai_agent组件的package.yaml中添加[example示例代码](https://gitee.com/alios-things/ai_agent/tree/master/example)：

```yaml
source_file:
  - "example/aiagent_example.c" # add aiagent_example.c
```

在ucloud_ai组件的package.yaml中添加：

```yaml
build_config:
 prebuild_script: cp_resources.py  #编译时cp_resources.py会对资源文件进行拷贝，系统自动打包资源文件到littlefs文件系统中
```

### 云端功能开通
1. 如没有阿里云账号，请登陆[阿里云官网](http://www.aliyun.com)开通。
2. 登陆[视觉智能开放平台](https://vision.aliyun.com)免费开通如下功能：

&emsp;&emsp;&emsp;&emsp;**[人脸人体](https://vision.aliyun.com/facebody)**
&emsp;**[文字识别](https://vision.aliyun.com/ocr)**
&emsp;**[分割抠图](https://vision.aliyun.com/imageseg)**
&emsp;**[目标检测](https://vision.aliyun.com/objectdet)**

3. 登陆[OSS平台](oss.console.aliyun.com)创建bucket：
```sh
a. 创建Bucket时地域选择“上海”
b. 读写权限选择“公共读”
c. Bucket名称全小写
```
### 配置OSS信息
在components/ucloud_ai/package.yaml中替换你的OSS信息

```yaml
OSS_ACCESS_KEY: "Your-Access-Key"
OSS_ACCESS_SECRET: "Your-Access-Secret"
OSS_ENDPOINT: "Your-OSS-Endpoint"    #例如: "oss-cn-shanghai-internal.aliyuncs.com"
OSS_BUCKET: "Your-OSS-Bucket"        #例如: "cloud-ai-dev"

OSS_ACCESS_KEY以及OSS_ACCESS_SECRET获取链接: https://usercenter.console.aliyun.com/#/accesskey
ENDPOINT默认使用“oss-cn-shanghai-internal.aliyuncs.com”，BUCKET请使用你创建好的Bucket名称。
```

## 步骤5 编译固件

在示例代码已经添加至组件的配置文件，并且helloworld_demo已添加了对该组件的依赖后，就可以编译helloworld_demo案例来生成固件了，具体编译方法可参考[《aos-studio使用说明之编译固件》](https://g.alicdn.com/alios-things-3.3/doc/build_project.html)。

## 步骤6 烧录固件

### 文件系统烧录
本组件例子中使用到到图片存放在代码中hardware/chip/haas1000/prebuild/data/目录下ucloud_ai_image目录，除烧录helloworld_demo image外，需烧录littlefs文件系统，请将hardware/chip/haas1000/package.yaml文件中以下代码段的注释打开：

```yaml
  program_data_files:
    - filename: release/write_flash_tool/ota_bin/littlefs.bin
      address: 0xB32000
```

上述步骤执行后，可参考[《aos-studio使用说明之烧录固件》](https://g.alicdn.com/alios-things-3.3/doc/burn_image.html)来烧录固件。

## 步骤7 打开串口

固件烧录完成后，可以通过串口查看示例的运行结果，打开串口的具体方法可参考[《aos-studio使用说明之查看日志》](https://g.alicdn.com/alios-things-3.3/doc/view_log.html)。

当串口终端打开成功后，可在串口中输入help来查看已添加的测试命令。


## 步骤8 示例测试

测试步骤：
```sh
$ aiagent -e ucloud-ai init  # 初始化aiagent组件
$ netmgr -t wifi -c {ssid} {password}  # 请将ssid修改为您路由器的WiFi名称，paasword填入路由器的WiFi密码
$ aiagent -e ucloud-ai -m {0 ~ 14}  # 测试AI用例
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai init  # 初始化ucloud-ai引擎，在执行下面的测试命令前，该命令需要优先执行，仅需执行一次即可
```

> CLI关键日志：
```sh
aiagent init successfully!
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 0  # 人脸比对
```

> CLI关键日志：
```sh
Facebody comparing result:
confidence: 84.6
location at x: 159, y: 76, w: 143, h: 194
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 1  # 人物动漫化
```

> CLI关键日志：
```sh
Generate human anime style result:
url: http://vibktprfx-prod-prod-aic-gd-cn-shanghai.oss-cn-shanghai.aliyuncs.com/person-image-cartoonizer/4F10FBB8-19DC-476B-A51D-458CD180A24F_07f2_20210331-142711.jpg?Expires=1617202631&OSSAccessKeyId=LTAI4FoLmvQ9urWXgSRpDvh1&Signature=YCLva1stK27Cn657nMeYhgFt7h4%3D
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 2  # 表情识别
```

> CLI关键日志：
```sh
Recognize expression result:
type: anger, probability: 1.0
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 3  # 目标检测
```

> CLI关键日志：
```sh
Detect object result:
type: potted plant, Score: 0.4, x: 35, y: 274, w: 185, h: 443
Detect object result:
type: potted plant, Score: 0.5, x: 663, y: 121, w: 847, h: 457
Detect object result:
type: monitor, Score: 0.6, x: 250, y: 43, w: 670, h: 439
Detect object result:
type: keyboard, Score: 0.5, x: 239, y: 435, w: 489, h: 485
Detect object result:
type: keyboard, Score: 0.3, x: 241, y: 428, w: 639, h: 506
Detect object result:
type: mouse, Score: 0.2, x: 660, y: 479, w: 750, h: 528
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 4  # 主体检测
```

> CLI关键日志：
```sh
Detect main body result:
main body location x: 1, y: 31, w: 461, h: 528
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 5  # 通用分割
```

> CLI关键日志：
```sh
Segment common image result:
url: http://viapi-cn-shanghai-dha-segmenter.oss-cn-shanghai.aliyuncs.com/upload/result_humansegmenter/2021-3-31/invi_humansegmenter_016172056326493133388_avpTpm.jpg?Expires=1617207432&OSSAccessKeyId=LTAI4FoLmvQ9urWXgSRpDvh1&Signature=WREOAoBUIvpLSyygGxKPfi82HfI%3D
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 6  # 面部分割
```

> CLI关键日志：
```sh
Segment face result:
image url: http://viapi-cn-shanghai-dha-segmenter.oss-cn-shanghai.aliyuncs.com/upload/result_HeadSegmenter/2021-3-31/invi_HeadSegmenter_016172056422754285967296_yRhj6C.png?Expires=1617207442&OSSAccessKeyId=LTAI4FoLmvQ9urWXgSRpDvh1&Signature=wR7GUxqrrwRF9u3mCJ%2BNDhAPCzk%3D
location at x: 153, y: 66, w: 160, h: 209
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 7  # 身份证证明识别
```

> CLI关键日志：
```sh
Recognize identity card face side result:
address: 江苏省南京市浦口区天天小区1栋11号
nationality: 汉
card location: x0: 1126563840, y0: 1126563840
card location: x1: 0, y1: 0
card location: x2: 0, y2: 0
card location: x3: 0, y3: 0
face location: x0: 1126563840, y0: 1126563840
face location: x1: 0, y1: 0
face location: x2: 0, y2: 0
face location: x3: 0, y3: 0
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 8  # 身份证反面识别
```

> CLI关键日志：
```sh
Recognize identity card back side result:
start date: 20131010
issue: 杭州市公安局余杭分局
end date: 20231010
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 9  # 银行卡识别
```

> CLI关键日志：
```sh
bankName: 中国工商银行
cardNumber: 6212262315007683105
validDate: 07/26
Recognize bank card result:
bank name: 中国工商银行
card number: 6212262315007683105
valid date: 07/26
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 10  # 文本识别
```

> CLI关键日志：
```sh
Recognize character result:
text: 《爆炸新闻》
probability: 1.0
text area: left: 141, top: 442, weight: 89, height: 20
Recognize character result:
text: 19
probability: 1.0
text area: left: 115, top: 173, weight: 100, height: 109
Recognize character result:
text: 豆瓣评分7.1
probability: 0.9
text area: left: 168, top: 447, weight: 98, height: 18
Recognize character result:
text: 2019||美国|加拿大|剧情|传记
probability: 0.5
text area: left: 181, top: 450, weight: 241, height: 17
Recognize character result:
text: 杰伊·罗奇 导演
probability: 0.9
text area: left: 216, top: 454, weight: 112, height: 18
Recognize character result:
text: 2021年2月星期五
probability: 1.0
text area: left: 231, top: 187, weight: 150, height: 18
Recognize character result:
text: 农历正月初八
probability: 1.0
text area: left: 261, top: 193, weight: 104, height: 18
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 11  # 垃圾分类
```

> CLI关键日志：
```sh
Recognize rubbish result:
rubbish: 塑料饮料瓶
rubbish score: 1.0
category: 可回收垃圾
category score: 1.0
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 12  # 水果检测
```

> CLI关键日志：
```sh
Recognize fruits result:
fruit name: 橙子
fruit score: 0.8
fruit location: x: 214, y: 44, w: 509, h: 438
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 13  # 图像人体擦除
```

> CLI关键日志：
```sh
Erase person result:
url: http://algo-app-isr-lab-cn-shanghai-prod.oss-cn-shanghai.aliyuncs.com/remove-person/2021-03-31_15%3A34%3A17.959810_person_org.jpg?Expires=1617206658&OSSAccessKeyId=LTAI4FoLmvQ9urWXgSRpDvh1&Signature=jFwwnSYOw2fXev1%2Fm5az4JBa%2B5Q%3D
```

**CLI命令行输入：**
```sh
aiagent -e ucloud-ai -m 14  # 风格迁移
```

> CLI关键日志：
```sh
Extend image style result:
out image url: http://viapi-cn-shanghai-dha-filter.oss-cn-shanghai.aliyuncs.com/upload/result_/2021-3-31/invi__016172056608421021961_1mdK2W.jpg?Expires=1617207460&OSSAccessKeyId=LTAI4FoLmvQ9urWXgSRpDvh1&Signature=3t8Q%2BwTAo6Lib9utaoMrWUJX6Iw%3D
```

# 注意事项
NA

# FAQ
NA

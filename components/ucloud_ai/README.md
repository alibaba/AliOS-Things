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

> ugraphics组件的package.yaml中添加编译配置:
```sh
build_config:
 prebuild_script: cp_resources.py
```

> helloworld_demo组件的application_start中添加代码
```sh
    /*init network service*/
    event_service_init(NULL);
    netmgr_service_init(NULL);
```

## 云端功能开通
> 登陆https://vision.aliyun.com免费开通如下功能：
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
## 配置OSS信息
> components/ucloud_ai/package.yaml中替换你的OSS信息
```sh
OSS_ACCESS_KEY: "Your-Access-Key"
OSS_ACCESS_SECRET: "Your-Access-Secret"
OSS_ENDPOINT: "Your-OSS-Endpoint"
OSS_BUCKET: "Your-OSS-Bucket"

KEY以及SECRET获取链接: https://usercenter.console.aliyun.com/#/accesskey
ENDPOINT使用默认即可，BUCKET请使用你创建好的Bucket名称

```

## 添加ucloud_ai组件
> helloworld_demo组件的package.yaml中添加如下组件：
```sh
depends:
  - ucloud_ai: dev_aos  # helloworld_demo中引入ucloud_ai组件
  - netmgr: dev_aos     # helloworld_demo中引入netmgr组件
  - littlefs: dev_aos   # helloworld_demo中引入littlefs组件
```

## 编译
```sh
cd solutions/helloworld_demo && aos make
```
## 资源文件打包
> 编译时ucloud_ai组件中cp_resources.py会对资源文件进行拷贝，系统自动打包到littlefs文件系统中。编译完成后请确认目录hardware/chip/haas1000/prebuild/data/下有ucloud_ai_image目录。

>hardware/chip/haas1000/prebuild/data/目录下如有其他不使用的文件，建议删除后再进行编译，避免littlefs不够用导致无法访问的问题。

> 在cli/package.yaml中设置打开CLI_IOBOX_ENABLE，便可在板子上通过ls命令查看资源文件是否成功烧录：
```sh
def_config:
  CLI_IOBOX_ENABLE: 1
```
```sh
ls /data/ucloud_ai_image
```

## 烧录固件
> helloworld_demo bin烧录：
```sh
aos burn
```

> littlefs文件系统烧录：
```sh
aos burn -f hardware/chip/haas1000/release/write_flash_tool/ota_bin/littlefs.bin#0xB32000
```
本组件例子中使用到到图片存放在代码中hardware/chip/haas1000/prebuild/data目录，除烧录helloworld demo image外，需烧录littlefs文件系统。

## ucloud_ai示例测试

### CLI命令行输入：
```sh
ucloud_ai -m 0  # 人脸比对
```

> CLI关键日志：
```sh
confidence:84.5607
x:159
y:76
w:143
h194
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 1  # 人物动漫化
```

> CLI关键日志：
```sh
image url:http://vibktprfx-prod-prod-aic-gd-cn-shanghai.oss-cn-shanghai.aliyuncs.com/person-image-cartoonizer/5BE7565D-59CE-43EE-8169-09ED2821CE21_b301_20210331-093707.jpg?Expires=1617185228&OSSAccessKeyId=LTAI4FoLmvQ9urWXgSRpDvh1&Signature=8zhYdqxKgz9eDz970l1BXmpPAiQ%3D
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 2  # 表情识别
```

> CLI关键日志：
```sh
face probablility:0.997393
x:872835160
y:873405208
w:4
h:872835304
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 3  # 目标检测
```

> CLI关键日志：
```sh
object num:6
object height:533
object width:948
object score:0.437
object type:potted plant
object boxes.x:35
object boxes.y:274
object boxes.w:185
object boxes.h:443
object score:0.487
object type:potted plant
object boxes.x:663
object boxes.y:121
object boxes.w:847
object boxes.h:457
object score:0.583
object type:monitor
object boxes.x:250
object boxes.y:43
object boxes.w:670
object boxes.h:439
object score:0.482
object type:keyboard
object boxes.x:239
object boxes.y:435
object boxes.w:489
object boxes.h:485
object score:0.261
object type:keyboard
object boxes.x:241
object boxes.y:428
object boxes.w:639
object boxes.h:506
object score:0.217
object type:mouse
object boxes.x:660
object boxes.y:479
object boxes.w:750
object boxes.h:528
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 4  # 主体检测
```

> CLI关键日志：
```sh
object x:56
object y:177
object width:661
object height:209
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 5  # 通用分割
```

> CLI关键日志：
```sh
0image url: http://viapi-cn-shanghai-dha-segmenter.oss-cn-shanghai.aliyuncs.com/upload/result_HeadSegmenter/2021-3-31/invi_HeadSegmenter_016171837626454285967296_L925gE.png?Expires=1617185562&OSSAccessKeyId=LTAI4FoLmvQ9urWXgSRpDvh1&Signature=bvMITW5gJQPlHBiP3Aa%2BO6fCdvQ%3D
0width: 160
0height: 209
0x: 153
0y: 66
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 6  # 面部分割
```

> CLI关键日志：
```sh
image url: http://viapi-cn-shanghai-dha-segmenter.oss-cn-shanghai.aliyuncs.com/upload/result_humansegmenter/2021-3-31/invi_humansegmenter_016171860047301086642_0V057V.jpg?Expires=1617187804&OSSAccessKeyId=LTAI4FoLmvQ9urWXgSRpDvh1&Signature=TiqmpoQqCon9KHPXEF8W6sf2vuY%3D
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 7  # 身份证证明识别
```

> CLI关键日志：
```sh
Address: 江苏省南京市浦口区天天小区1栋11号
BirthDate:
gender:
nationality: 汉
iDNumber:
faceRectangle.x: 663.147
faceRectangle.y: 249.947
faceRectangle.width: 115.432
faceRectangle.height: 96.2586
faceRectangle.angle: -87.3476
0cardAreas.x: 166
0cardAreas.y: 97
0faceRectVertices.x: 708.554
0faceRectVertices.y: 309.828
1cardAreas.x: 775
1cardAreas.y: 97
1faceRectVertices.x: 612.398
1faceRectVertices.y: 305.374
2cardAreas.x: 775
2cardAreas.y: 460
2faceRectVertices.x: 617.74
2faceRectVertices.y: 190.065
3cardAreas.x: 166
3cardAreas.y: 460
3faceRectVertices.x: 713.895
3faceRectVertices.y: 194.52
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 8  # 身份证反面识别
```

> CLI关键日志：
```sh
startDate: 20131010
issue: 杭州市公安局余杭分局
endDate: 20231010
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 9  # 银行卡识别
```

> CLI关键日志：
```sh
bankName: 中国工商银行
cardNumber: 6212262315007683105
validDate: 07/26
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 10  # 文本识别
```

> CLI关键日志：
```sh
results size: 7
0text: 《爆炸新闻》
0probability: 0.987366
0text left: 442
0text angle: -9
0text top: 141
0text height: 20
0text: width:89
1text: 19
1probability: 0.993373
1text left: 173
1text angle: -10
1text top: 115
1text height: 109
1text: width:100
2text: 豆瓣评分7.1
2probability: 0.896118
2text left: 447
2text angle: -9
2text top: 168
2text height: 18
2text: width:98
3text: 2019||美国|加拿大|剧情|传记
3probability: 0.494584
3text left: 450
3text angle: -9
3text top: 181
3text height: 17
3text: width:241
4text: 杰伊·罗奇 导演
4probability: 0.894469
4text left: 454
4text angle: -9
4text top: 216
4text height: 18
4text: width:112
5text: 2021年2月星期五
5probability: 0.991614
5text left: 187
5text angle: -9
5text top: 231
5text height: 18
5text: width:150
6text: 农历正月初八
6probability: 0.984329
6text left: 193
6text angle: -9
6text top: 261
6text height: 18
6text: width:104
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 11  # 垃圾分类
```

> CLI关键日志：
```sh
sensitive: 0
0rubbish: 塑料饮料瓶
0category: 可回收垃圾
0categoryScore: 1
0rubbishScore: 1
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 12  # 水果检测
```

> CLI关键日志：
```sh
0score: 0.822785
0name: 橙子
box.xmin: 214.05966186523438
box.ymin: 44.830513000488281
box.xmax: 723.5120849609375
box.ymax: 482.88070678710938
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 13  # 图像人体擦除
```

> CLI关键日志：
```sh
imageUrl: http://algo-app-isr-lab-cn-shanghai-prod.oss-cn-shanghai.aliyuncs.com/remove-person/2021-03-31_10%3A20%3A46.011134_person_org.jpg?Expires=1617187846&OSSAccessKeyId=LTAI4FoLmvQ9urWXgSRpDvh1&Signature=OwLEg5pWNVDwXrvAbGsUU7q5Dbs%3D
ucloud_ai comp test success!
```

### CLI命令行输入：
```sh
ucloud_ai -m 14  # 风格迁移
```

> CLI关键日志：
```sh
outMajorURL:
outImageURL: http://viapi-cn-shanghai-dha-filter.oss-cn-shanghai.aliyuncs.com/upload/result_/2021-3-31/invi__016171860565631021659_XIqH26.jpg?Expires=1617187856&OSSAccessKeyId=LTAI4FoLmvQ9urWXgSRpDvh1&Signature=tqJqHHoPSvga6LyET31TZsvOXFs%3D
ucloud_ai comp test success!
```

# 注意事项
使用过程中如果想要验证自己的图片是否可以正常识别可通过http://vision.aliyun.com进入能力中心中对应的体验页面进行验证。

# FAQ
如果遇到：
> error code: InvalidAction.NotFound
请确认Endpoint是否设置正确

> error code: InvalidApi.NotPurchase

请确认是否在vision.aliyun.com开通相应功能

> tlefs_vfs.c:677:error: lfs_vfs_open /data/ucloud_ai_image/object.jpg failed, ret - -2

请确认是否有拷贝图片到hardware/chip/haas1000/prebuild/data/ucloud_ai_image目录。
```sh
1. 在cli/package.yaml中设置CLI_IOBOX_ENABLE: 1
2. ls /data/ucloud_ai_image查看
```

如果有，可能是文件系统存储空间不够，此时请执行：
1. 在ucloud_ai/package.yaml中屏蔽prebuild_script: cp_resources.py；
2. 删除ucloud_ai_image中其他文件，仅保留测试case使用的图片文件；
3. 再重新编译烧录。

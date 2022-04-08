# 垃圾分类识别系统
&emsp;&emsp;
本系统在物体接近后，通过红外避障传感器检测到物体，然后启动摄像头抓拍图片上传至云端，云端通过HaaS增值服务对抓拍的图片进行识别，最后给出图片中的物品是属于那种垃圾，同时给出识别物品名称和可信度，操作者可根据识别结果，把垃圾投放至对应垃圾箱。

<div align="center">
<img src=./../../../images/classifying_rubbish/垃圾分类识别_HaaS云端组件_步骤指引.png width=80%/>
</div>


## 背景知识

&emsp;&emsp;
红外避障传感器：利用物体的反射性质，在一定范围内，如果没有障碍物，发射出去的红外线，距离越远越弱，如果有障碍物，红外线遇到障碍物就会被反射，传感器接收头接收到这一信号，就可以确定正前方有物体。

&emsp;&emsp;
HaaS增值服务：集多种人工智能算法为一体的云端服务，通过设备端上传图片到云端，云端对图片进行识别，并把结果返回给设备的服务，目前已涵盖图片识别、文字识别、物品识别、人型识别等多个类别。

&emsp;&emsp;
垃圾分类目前分为可回收垃圾、有害垃圾、厨余垃圾(湿垃圾)、其他垃圾(干垃圾)。
<br>

常见垃圾输出列举值如下：
### 可回收垃圾
废纸张：纸板箱、报纸、废弃书本、快递纸袋、打印纸、信封、广告单、纸塑铝复合包装（利乐包）。
废塑料：食品与日用品塑料瓶罐及瓶盖（饮料瓶、奶瓶、洗发水瓶、乳液罐）、食用油桶、塑料碗（盆）、塑料盒子（食品保鲜盒、收纳盒）、塑料玩具（塑料积木、塑料模型）、塑料衣架、施工安全帽、PE塑料、pvc、亚克力板、塑料卡片、密胺餐具、kt板、泡沫（泡沫塑料、水果网套）。
废玻璃制品：食品及日用品玻璃瓶罐（调料瓶、酒瓶、化妆品瓶）、玻璃杯、窗玻璃、玻璃制品（放大镜、玻璃摆件）、碎玻璃。
废金属：金属瓶罐（易拉罐、食品罐/桶）、金属厨具（菜刀、锅）、金属工具（刀片、指甲剪、螺丝刀）、金属制品（铁钉、铁皮、铝箔）。
废织物：旧衣服、床单、枕头、棉被、皮鞋、毛绒玩具（布偶）、棉袄、包、皮带、丝绸制品。
其它：电路板（主板、内存条）、充电宝、电线、插头、木制品（积木、砧板）。

### 有害垃圾
废镍镉电池和废氧化汞电池：充电电池、镉镍电池、铅酸电池、蓄电池、纽扣电池。
废荧光灯管：荧光（日光）灯管、卤素灯。
废药品及其包装物：过期药物、药物胶囊、药片、药品内包装。
废油漆和溶剂及其包装物：废油漆桶、染发剂壳、过期的指甲油、洗甲水。
废矿物油及其包装物。
废含汞温度计、废含汞血压计：水银血压计、水银体温计、水银温度计。
废杀虫剂及其包装：老鼠药（毒鼠强）、杀虫喷雾罐。
废胶片及废相纸：x光片等感光胶片、相片底片。

### 厨余垃圾（湿垃圾）
食材废料：谷物及其加工食品（米、米饭、面、面包、豆类）、肉蛋及其加工食品（鸡、鸭、猪、牛、羊肉、蛋、动物内脏、腊肉、午餐肉、蛋壳）、水产及其加工食品（鱼、鱼鳞、虾、虾壳、鱿鱼）、蔬菜（绿叶菜、根茎蔬菜、菌菇）、调料、酱料。
剩菜剩饭：火锅汤底（沥干后的固体废弃物）、鱼骨、碎骨、茶叶渣、咖啡渣。
过期食品：糕饼、糖果、风干食品（肉干、红枣、中药材）、粉末类食品（冲泡饮料、面粉）、宠物饲料。
瓜皮果核：水果果肉（椰子肉）、水果果皮（西瓜皮、桔子皮、苹果皮）、水果茎枝（葡萄枝）、果实（西瓜籽）。
花卉植物：家养绿植、花卉、花瓣、枝叶。
中药药渣。

### 其他垃圾（干垃圾）
餐巾纸、卫生间用纸、尿不湿、猫砂、狗尿垫、污损纸张、烟蒂、干燥剂、污损塑料、尼龙制品、编织袋、防碎气泡膜、大骨头、硬贝壳、硬果壳（椰子壳、榴莲壳、核桃壳、玉米衣、甘蔗皮）、硬果实（榴莲核、菠萝蜜核）、毛发、灰土、炉渣、橡皮泥、太空沙、带胶制品（胶水、胶带）、花盆、毛巾、一次性餐具、镜子、陶瓷制品、竹制品（竹篮、竹筷、牙签）、成分复杂的制品（伞、笔、眼镜、打火机）。

## 场景功能拆解
&emsp;&emsp;
实验整体步骤如下：
1. 连接硬件
2. HaaS云端积木平台创建设备
3. 设备端开发
4. 设备运行

## 准备

1. M5Stack Core2开发板       一套
2. M5Stack Unit CAM摄像头    一个
3. 连接线                    一根
4. 红外避障传感器             一个
5. 公对母杜邦线               若干

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！

| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| M5Stack Core2开发版 | 1 | [M5Stack Core2](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.17.732749d8usCqYX&id=625561056791) |
| microUSB数据线 | 1 | M5Stack Core2开发套件自带 |
| 摄像头 | 1 | [M5Stack Unit CAM摄像头](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.29.698e2d4844EBZF&id=643872470244) |
| 连接线 | 1条 | [M5Stack GROVE连接线](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22404213529.11.6b6d5f86B5IYMF&id=610410604759) 请选用10cm长即可 |
| 红外避障传感器 | 1 | [红外避障传感器](https://detail.tmall.com/item.htm?id=40844279989&ali_refid=a3_430582_1006:1109448581:N:10dd5ZFgNCY07xv6IBZLPW%20Fztb96tqA:cd36f5dbea54931aaabadac652033624&ali_trackid=1_cd36f5dbea54931aaabadac652033624&spm=a230r.1.14.6)  |
| 公对母杜邦线 | 若干 | [公对母杜邦线](https://detail.tmall.com/item.htm?spm=a230r.1.14.22.b10c6663PCvNOq&id=41254478179&ns=1&abbucket=7&skuId=3211140814458) 请选用10cm长即可 |

&emsp;&emsp;
连线示意图如下图所示：
<div align="center">
<img src=./../../../images/classifying_rubbish/垃圾分类识别_设备布局.png width=30%/>
</div>

&emsp;&emsp;
硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/classifying_rubbish/垃圾分类识别_设备连线图.png width=80%/>
</div>
<br>

## 云端平台功能开通
在本案例中涉及到云端平台功能都聚合在HaaS云端积木中，所以无需单独使用 **对象存储OSS** 和 **视觉智能开发平台**。因此，整个云端步骤分为下面两步。
```python
1. 注册阿里云账号并登录阿里云账号；
2. 登录HaaS云端积木控制台创建设备；
```

## 注册阿里云账号
&emsp;&emsp;
进入阿里云官网，如已有阿里云账号可跳过这步，[注册阿里云账号](https://account.aliyun.com/login/login.htm) 。

<br>

## HaaS云端积木平台创建设备

1. **登录HaaS云端积木平台**

&emsp;&emsp;
进入阿里云官网，用阿里云账号[登录HaaS云端组件控制台](https://haas.iot.aliyun.com/welcome) 。
<div align="center">
<img src=./../../../images/classifying_rubbish/AI_HaaS官网.png width=90%/>
</div>
<br>


2. **创建设备**

&emsp;&emsp;
在左侧导航栏设备管理中，可以看到产品和设备选项，点击“产品”，可以看到平台默认创建了“haas_正式游客产品”这个产品；
所以开发者只需要创建设备即可。点击“批量添加”， 添加方式为“自动生成”， 申请数量可填为“1”个，即可完成新设备的创建。
<div align="center">
<img src=./../../../images/classifying_rubbish/AI_创建设备.png width=60%/>
</div>
<div align="center">
<img src=./../../../images/classifying_rubbish/AI_创建设备_设备信息.png width=60%/>
</div>
<br>

3. **获取设备的三元组**

&emsp;&emsp;
在设备的列表中，新创建的设备上点击“查看”,就可以获得设备的三元组信息，之后这个三元组信息会在设备端代码中使用到。
<div align="center">
<img src=./../../../images/classifying_rubbish/AI_查看设备信息.png width=80%/>
</div>
<div align="center">
<img src=./../../../images/classifying_rubbish/AI_设备三元组.png width=80%/>
</div>
<br>

&emsp;&emsp;
请复制该处的三元组信息，后面设备端开发需要使用。

## 设备端开发

1. **开发环境**

&emsp;&emsp;
在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[M5Stack Core2开发环境](https://haas.iot.aliyun.com/haasapi/index.html?spm=a2cpu.b16145223.0.0.595660b1exaiyq#/Python/docs/zh-CN/startup/M5StackCore2_startup)说明。
<br>

2. **创建解决方案**

&emsp;&emsp;
如下图所示，打开VSCode之后在新建一个基于helloworld的python工程，设定好工程名称（“classifying_rubbish”）及工作区路径之后，硬件类型选择M5Stack Core2，点击立即创建，创建一个Python轻应用的解决方案。

<div align="center">
<img src=./../../../images/classifying_rubbish/垃圾分类识别_创建工程.png width=80%/>
</div>


1. **代码准备**

&emsp;&emsp;
将[垃圾分类识别代码](./code/)文件下的所有脚本进行复制到“classifying_rubbish”工程根目录中，然后进行如下设定完成设备端代码的开发。
> Python脚本的详细说明请参考脚本内嵌的文字注释


3.1. **修改路由器名称及密码**

&emsp;&emsp;
修改classifying_rubbish工程里main.py中SSID和PWD的值为读者实际要连接的路由器的名称及密码（请注意名称和密码都需要放在''符号中间）。

```python
# Wi-Fi SSID和Password设置
SSID='Your-AP-SSID'
PWD='Your-AP-Password'
```

&emsp;&emsp;
修改完成之后connect_wifi函数就会连接读者自己设定的路由器。

3.2. **修改设备的三元组信息**

&emsp;&emsp;
请参考“获取设备的三元组步骤”，把获取的三元组信息，填入main.py中：
```python
# HaaS设备三元组
productKey = "Your-ProductKey"
deviceName  = "Your-devicename"
deviceSecret  = "Your-deviceSecret"
```

3.3. **运行结果**

&emsp;&emsp;
推送此脚本到M5Stack Core2之后，网络连接成功后，屏幕上将打印出IP地址和"NTP Done"，如果迟迟不能显示“NTP Done”，请长按电源键关机同时拔掉USB线重新上电运行。

&emsp;&emsp;
把要识别的垃圾放到大约距离红外传感器1-10cm处，当传感器上两个灯都亮起时，表明检测到物体开始垃圾分类识别，识别成功后在屏幕上显示红色XXXX Detected的字样，XXXX是四种垃圾分类的一种，样例log如下。

```log
I (71364) phy: phy_version: 4500, 0cd6843, Sep 17 2020, 15:37:07, 0, 0
Wi-Fi is connecting...
Wi-Fi is connecting...
Wi-Fi is connected
IP: 192.168.xxx.xxx
NTP start
NTP done
E (61287) ACTIVATION_REPORT: activation success

establish tcp connection with server(host='${Your-ProductKey}.iot-as-mqtt.cn-shanghai.aliyuncs.com', port=[443])
tcp_connect: can only connect from state CLOSED
success to establish tcp, fd=54
link platform connected
IR detector ...
启动红外检测线程
有物体进入
开始识别垃圾
[  64.140]<E>AIOT_UPLOAD MQTT Upload file(rubbish.jpg) ID(VNCESFk8cDUP1ai65JTZ010200) success
Recyclable 玻璃杯
识别结束
```

<div align="center">
<img src=./../../../images/classifying_rubbish/垃圾分类识别_识别成功.png width=80%/>
</div>

## 后记

&emsp;&emsp;
本案例使用了三个线程，有兴趣的同学，可以尝试减少线程来实现同样目标。
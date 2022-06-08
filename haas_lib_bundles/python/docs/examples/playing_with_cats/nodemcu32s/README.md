# 自动逗猫机

# 1、简介
## 1.1、背景
养宠物猫的家庭经常遇到“逗猫”的需求：使用逗猫棒或者激光笔引导猫做出类似捕猎的动作，起到锻炼、消耗体力和精力的效果。如果不经常逗猫，精力过剩的宠物猫往往会在夜晚或主人不在家的时候做出各种破坏家具的行为。
<br>
本案例使用激光笔、继电器、步进电机等设备在地面上形成闪烁移动的光点，激发宠物猫的捕猎本能，达到无人操作自动逗猫的效果。
<div align="center">
<img src=./../../../images/playing_with_cats/cat.gif width=50%/>
</div>
<br>

## 1.2、准备
1. NodeMCU32S开发板一套<br>
2. ULN2003步进电机控制器一个<br>
3. 28BYJ4步进电机、减速器一套<br>
4. 继电器一个<br>
5. 激光笔<br>
6. 母对母杜邦连接线若干<br>

涉及到的硬件购买链接如下，仅供参考，不负责商家发货的品质保障等问题！
<br>


| 名称 | 数量 | 参考链接 |
| --- | --- | --- |
| NodeMCU32S开发板 | 1 | [NodeMCU32S开发板](https://haas.iot.aliyun.com/solution/detail/hardware?spm=a2cpu.b17028610.0.0.6c2360b1yDodxD&versionId=800C9562896F994200000001&dataId=800C9562896F9942)|
| ULN2003步进电机控制器 | 1 | [ULN2003步进电机控制器](https://detail.tmall.com/item.htm?id=41303683115) |
| 28BYJ4步进电机、减速器 | 1 | [28BYJ4步进电机、减速器](https://detail.tmall.com/item.htm?id=41303683115) |
| 继电器 | 1 | [继电器](https://detail.tmall.com/item.htm?id=530650857798) |
| 激光笔 | 1 | [激光笔](https://item.taobao.com/item.htm?id=541521090282) |
| 母对母杜邦线 | 9 | [母对对母杜邦线](https://detail.tmall.com/item.htm?id=14466195609&ali_refid=a3_430582_1006:1104520036:N:MsF9mE9KLTC2IibWJh+K1A==:21c5b519e28c4b0cd17ec4383141281f&ali_trackid=1_21c5b519e28c4b0cd17ec4383141281f&spm=a230r.1.14.1&skuId=3108214440213) 请选用10cm长即可 |

硬件连线图如下图所示：
<div align="center">
<img src=./../../../images/playing_with_cats/hardware_connection.png width=100%/>
</div>
<br>

## 2、设备端开发

### 2.1、开发环境

在进行下一步之前请确保ESP32开发环境已经搭建完毕。详情请参考[ESP32开发环境](../../../startup/ESP32_startup.md)的说明。
<br>

### 2.2、创建解决方案

&emsp;&emsp;
如下图所示，在Haas Studio中创建项目。先选择左侧的“开发板型号”再从右侧的案例中选择“自动逗猫机”案例点击“立即创建”即可。
<div align="center">
<img src=./../../../images/HaaS_Studio_创建工程示范.png width=100%/>
</div>
<br>

> Python脚本的详细说明请参考脚本内嵌的文字注释

## 3、运行结果

将本工程推送到开发板中，案例运行后会在地面上形成闪烁移动的光点。如果周围有宠物猫，它会一直试图捕捉光点，达到逗猫的效果。
<br>

到此为止，自动逗猫机案例就已经完成了。如果想学习更多实验，请参考[HaaS案例实践详解](https://gitee.com/haasedu/haasedu/tree/release_2.0)。

## 4、后记

后续可加入定时动作、云端控制、图像识别等功能，形成一套针对宠物店的解决方案。

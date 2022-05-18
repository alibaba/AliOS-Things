# 紫外线传感器 - uv

## 一、产品简介
紫外线是电磁波谱中波长从10nm到400nm辐射的总称。波长的不同，一般把紫外线分为A、B、C三个波段，具体如下：
* UVA：400～315 nm
* UVB：315～280 nm
* UVC：280～100 nm

紫外线传感器可以利用光敏元件通过光伏模式和光导模式将紫外线信号转换为可测量的电信号，然后通过对电信号进行转换得出紫外线的强度。紫外线传感器常用在环境监测、气象监测、农业、林业等环境中测量大气中以及人造光源等环境下的紫外线强度。其具有高灵敏度、高输 出、高响应速度等特性 ， 并且抗干扰能力强 、稳定可靠 、寿命长 、耗电少。

本文介绍的紫外线传感器外如下图所示：

<div align="center">
<img src=./../../docs/images/uv_外观图.png  width=20%/>
</div>

其管脚定义如下表所示：
|管脚名称|管脚功能|说明|
|:-|:-|:-|
|VCC|电源|3.3V-5V|
|GND|地|-|
|OUT|紫外线测量结果输出|0~1.1V，分别对应1~11级|

## 二、紫外线传感器技术参数
* 工作电压：3.0v~5v<br>
* 输出电压：0~1.1V（对应1~11级）<br>
* 响应波长：200nm~370nm
* 测量精度：+-1UV
* 工作温度：-20°C～+85°C<br>
* 通信接口：ADC<br>
* 响应时间：<0.5秒<br>
* 工作电流：0.06~0.1mA<br>
* 尺寸：19.8mm * 15mm<br>

## 三、软件接口

紫外线传感器HaaS Python驱动：[下载地址](https://github.com/alibaba/AliOS-Things/tree/master/haas_lib_bundles/python/libraries/uv)
<br>

### UV(adcObj) - 创建UV传感器驱动对象
* 函数原型：
> gyroDev = UV(adcObj)

* 参数说明：

|参数|类型|必选参数？|说明|
|-----|----|:---:|----|
|adcObj|ADC|是|调用此函数前需确保adcObj对象已经处于open状态|

* 返回值：
执行成功，返回紫外线传感器对象；创建失败，抛出Exception

* 示例代码：

```python
from driver import ADC

adcObj = ADC()
adcObj.open("uvSensor")

UVDev = UV(adcObj)           # 创建UV传感器对象
print('UV device init done')
```

* 输出：
```log
UV device init done
```

### measureUVLevel - 测量紫外线级别
* 函数原型：
> UV.measureUVLevel()

* 参数说明：
无

* 返回值：
返回紫外线的级别，单位：mV，范围：0~1.1。

* 示例代码：

```python
from driver import ADC

adcObj = ADC()
adcObj.open("uvSensor")

UVDev = UV(adcObj)
print('UV device init done')

uvLevel = UVDev.measureUVLevel()
print("UV level:%d", uvLevel)
```

* 输出：
<xx> <yy> <zz>分别代表x,y,z三个方向的实际加速度测量值
```log
UV device init done
UV level:0.1
```

<br>

## 五、工作原理

紫外线传感器又叫紫外光敏管，是一种利用光电子发射效应的光电管，它能响应并检测300nm以下紫外辐射。

目前国内外有使用价值的紫外管可以按阴极形状分为球形、丝形、平板形结构，均为二极管的电极结构形式。紫外光敏管利用阴极表面的光电子发射效应，在管内封入特殊的气体，作为放电管的工作媒介。
如果对紫外线敏管的光电阴极和阳极之间加电压，如果有紫外线透过玻壳照射到阴极上.由于阴极涂敷有电子放射物质，阴极就会发射光电子。在电场的作用下光电子被吸向阳极，光电子高速运动时与管内气体分子相碰撞而使气体分子电离，气体电离产生的电子再与气体分子相碰撞，最终使阴极和阳极间被大量的光电子和离子所充斥，引起辉光放电现象，电路中生成大的电流。当没有紫外线照射时，阴极和阳极间没有电子和离子的流动，呈现出相当高的阻抗。

## 参考文献及购买链接
[1] [详解紫外线传感器的三大结构分类](https://zhuanlan.zhihu.com/p/1736813175)<br>
[2] [购买链接](https://detail.tmall.com/item.htm?id=601443043844)


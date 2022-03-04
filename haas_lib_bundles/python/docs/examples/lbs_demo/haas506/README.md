# lbs位置服务


## 1.简介
&emsp;&emsp;
除了像GPS这种卫星定位之外，我们身边的设备如：智能手机、智能手表、穿戴设备等联网的设备都可以用来定位，通过连接运营商（电信、联通、移动）基站来定位的方式称之为基站定位，或者又称之为一点位置服务（LBS--Location Based Service)。本案例使用Haas506开发板，使用高德地图的位置服务，来获取相关的位置信息。


## 2.程序修改

&emsp;&emsp;在高德地图中创建一个应用，获取一个key,填入到lbs.py的__init__中的相应位置上。需要注意的是，个人用户无法创建“智能设定位“应用。
```python
    def __init__(self):
        self.requesturl = ''
        self.url = 'http://apilocate.amap.com/position'
        self.accesstype = 0
        self.key = 'yourKey'
        self.cdma = 0
        self.output = 'json'
        self.bts = ''
        self.nearbts = ''
        self.htp = http.client()
        pass 
```

## 3.运行结果

```python
content: {'result': {'citycode': '0512', 'type': '4', 'city': '苏州市', 'poi': '苏州工业园区云彩创新孵化器', 'desc': '江苏省 苏州市 虎丘区 仁爱路 靠近苏州工业园区云彩创新孵化器', 'adcode': '320505', 'street': '仁爱路', 'radius': '550', 'location': '120.7429338,31.2754031', 'road': '仁爱路', 'country': '中国', 'province': '江苏省'}, 'status': '1', 'infocode': '10000', 'info': 'OK'}
('120.7429338', '31.2754031')
     
```


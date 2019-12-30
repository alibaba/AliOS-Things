# HTU21D

###Driver功能: 温湿度传感器
htu21d是一款温湿度采集传感器，具有很高的精度；


###硬件资源

1.DevelopKit开发板;

2.htu21d模块;

3.接线

HTU21D模块 SCL 引脚接 DevelopKit I2C2的SCL引脚；

HTU21D模块 SDA 引脚接 DevelopKit I2C2的SDA引脚；

HTU21D模块 VCC引脚接 DevelopKit VCC引脚；

HTU21D模块 GND引脚接 DevelopKit GND引脚；
 


###Driver配置
DevelopKit：

```
  "htu21d":{
    "type":"I2C",
    "port":2,
    "address_width":7,
    "freq":100000,
    "mode":1,
    "dev_addr":128
  }
```


###API说明
```
class：htu21d
param：@id  i2c端口对应的id，对应board.json中的htu21d;
method:
      getTemperature()  获取温度值。
      getHumidity() 获取湿度值。

```

###示例
每间隔1s读取温度和湿度的值:

```
var htu21d = require('htu21d/src/index.js');
var handle = new htu21d('htu21d');
handle.reset();

var getVal = function(){
  var temp = handle.getTemperature();
  var hum = handle.getHumidity();
  console.log('temp:'+temp + ' hum:'+hum);
}

var t = setInterval(getVal, 1000);
```








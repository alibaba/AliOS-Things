# Si7021

###Driver功能: 温湿度传感器
Si7021是一款温湿度传感器，其湿度测量范围是0-100%RH，温度测量范围为-40-85%。



###硬件资源

1.DevelopKit开发板;

2.Si7021模块;

3.接线

Si7021模块 SCL 引脚接 DevelopKit I2C2的SCL引脚；

Si7021模块 SDA 引脚接 DevelopKit I2C2的SDA引脚；
 
Si7021模块 VCC引脚接 DevelopKit VCC引脚；

Si7021模块 ADD0引脚接 DevelopKit VCC引脚；
 


###Driver配置
DevelopKit：

```
  "si7021":{
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
class：si7021
param：@id  i2c端口信息，对应board.json中的si7021;
method:
      getHumi() 获取湿度值;
      getTemperature() 获取温度值;

```

###示例
每间隔2s获取温度和湿度值:

```
console.log('app for si7021 ');
var si7021 = require('si7021/src/index.js');
var handle = new si7021('si7021');

var getVal = function(){
  var temp = handle.getTemperature();
  console.log('Temperature:'+temp);
  var humi = handle.getHumi();
  console.log('humi:'+humi);

}

var t = setInterval(getVal, 2000);
```








# ms5661

###Driver功能: 气压传感器
ms5661是一款大气压强传感器，内置24bit的AD转换器；


###硬件资源

1.DevelopKit开发板;

2.ms5661模块;

3.接线
MS5661模块 SCL 引脚接 DevelopKit I2C2的SCL引脚；

MS5661模块 SDA 引脚接 DevelopKit I2C2的SDA引脚；

MS5661模块 VCC引脚接 DevelopKit VCC引脚；

MS5661模块 GND引脚接 DevelopKit GND引脚；

MS5661模块 CSB引脚接 DevelopKit VCC引脚；
 
###Driver配置
DevelopKit示范：

```
  "ms5611":{
    "type":"I2C",
    "port":2,
    "address_width":7,
    "freq":100000,
    "mode":1,
    "dev_addr":236
  }
```


###API说明
```
class：mpu9250
param：@id  i2c端口配置信息，对应board.json中的mpu9250;

method:
      getAcc() 获取加速度xyz轴裸数据;
      getGyro() 获取陀螺仪xyz轴裸数据;
```


###示例
每间隔1s读取温度和气压的raw数据：

```
var ms5611 = require('ms5611/src/index.js');
var handle = new ms5611('ms5611');
handle.reset();

var getVal = function(){
  var temp = handle.getTemperature();
  var pressure = handle.getPressure();
  console.log('temp:'+temp + ' pressure:'+pressure);
}

var t = setInterval(getVal, 1000);
```








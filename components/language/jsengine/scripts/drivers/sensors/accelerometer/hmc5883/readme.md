# HMC5883


###Driver功能: 加速度传感器

###硬件资源

1.DevelopKit开发板;

2.HMC5883模块;

3.接线

HMC5883模块 SCL 引脚接 DevelopKit I2C2的SCL引脚；

HMC5883模块 SDA 引脚接 DevelopKit I2C2的SDA引脚；

HMC5883模块 VCC引脚接 DevelopKit VCC引脚；

HMC5883模块 3.3V引脚接 DevelopKit VCC引脚；
 

###Driver配置
DevelopKit：

```
  "hmc5883":{
    "type":"I2C",
    "port":2,
    "address_width":7,
    "freq":100000,
    "mode":1,
    "dev_addr":60
  }
```


###API说明
```
class：hmc5883
param：@id  i2c端口配置id，对应board.json中的hmc5883;
method:
      getAcc() 获取加速度xyz的raw数据。

```


###示例

每间隔2s读取加速度的raw数据值:

```
var hmc5883 = require('hmc5883/src/index.js');
var handle = new hmc5883('hmc5883');

var getVal = function(){
  var val = handle.getAcc();
  console.log('XH:'+val[0]);
  console.log('XL:'+val[1]);
  console.log('YH:'+val[2]);
  console.log('YL:'+val[3]);
  console.log('ZH:'+val[4]);
  console.log('ZL:'+val[5]);
}

var t = setInterval(getVal, 2000);

```








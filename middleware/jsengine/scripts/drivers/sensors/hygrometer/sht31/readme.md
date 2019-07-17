# SHT31

###Driver功能: 温湿度传感器



###硬件资源

1.DevelopKit开发板;

2.SHT31模块;

3.接线

SHT31模块 SCL 引脚接 DevelopKit I2C2的SCL引脚；

SHT31模块 SDA 引脚接 DevelopKit I2C2的SDA引脚；

SHT31模块 VCC引脚接 DevelopKit VCC引脚；

SHT31模块 ADD0引脚接 DevelopKit VCC引脚；
 




###Driver配置
DevelopKit：

```
  "sht31":{
    "type":"I2C",
    "port":2,
    "address_width":7,
    "freq":100000,
    "mode":1,
    "dev_addr":136
  }
```

###API说明
```
class：sht31
param：@id  i2c端口信息，对应board.json中的sht31;
method:
      getHumTem() 获取湿度和温度值;
```


###示例
每间隔2s读取环境的温度和湿度值:

```
console.log('app for sht31! ');
var sht31 = require('sht31/src/index.js');
var handle = new sht31('sht31');

var getVal = function(){
  var val = handle.getHumTem();
  console.log('tem:'+val[0] + ' hum:'+val[1]);
}

var t = setInterval(getVal, 2000);
```



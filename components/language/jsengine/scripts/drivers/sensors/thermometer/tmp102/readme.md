# TMP102

###Driver功能: 温度传感器



###硬件资源

1.DevelopKit开发板;

2.tmp102模块;

3.接线
TMP102模块 SCL 引脚接 DevelopKit I2C2的SCL引脚；

TMP102模块 SDA 引脚接 DevelopKit I2C2的SDA引脚；

TMP102模块 VCC引脚接 DevelopKit VCC引脚；

TMP102模块 ADD0引脚接 DevelopKit VCC引脚；

TMP102模块 GND引脚接 DevelopKit GND引脚；

 

###Driver配置
DevelopKit示范：

```
  "tmp102":{
    "type":"I2C",
    "port":2,
    "address_width":7,
    "freq":100000,
    "mode":1,
    "dev_addr":146
  }
```


###API说明
```
class：tmp102
param：@id  i2c端口配置信息，对应board.json中的tmp102;
method:
      getVal() 获取Do状态值

```

## 示例
每间隔2s获取温度值：

```
console.log('app for tmp102!');
var tmp102 = require('tmp102/src/index.js');
var handle = new tmp102('tmp102');


var getVal = function(){
  var temp = handle.getTemperature();
  console.log('Temperature:'+temp);

}

var t = setInterval(getVal, 2000);
```








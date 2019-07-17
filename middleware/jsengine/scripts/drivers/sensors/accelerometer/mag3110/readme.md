# Mag3110

###Driver功能: 加速度传感器
Mag3110是一款低功耗的加速度传感器。


###硬件资源

1.DevelopKit开发板;

2.Mag3110模块;

3.接线
 
 MAG3110模块 SCL 引脚接 DevelopKit I2C2的SCL引脚；
 
 MAG3110模块 SDA 引脚接 DevelopKit I2C2的SDA引脚；
 
 MAG3110模块 VCC引脚接 DevelopKit VCC引脚；
 
 MAG3110模块 3.3V引脚接 DevelopKit VCC引脚；
 




###Driver配置

DevelopKit：

```
  "mag3110":{
    "type":"I2C",
    "port":2,
    "address_width":7,
    "freq":100000,
    "mode":1,
    "dev_addr":28
  }
```


###API说明

```
class：mag3110
param：@id  i2c端口对应的id信息，对应board.json中的mag3110;
method:
      getAcc() 获取加速度raw数据;

```

###示例

读取加速度的raw数据值:

```
var mag3110 = require('mag3110/src/index.js');
var handle = new mag3110('mag3110');

var getVal = function(){
  var temp = handle.getAcc();
  var check = temp[0] + temp[1] + temp[2] +temp[3] + temp[4] + temp[5];
  if(0 == check)return;

  console.log('xL:'+temp[0]);
  console.log('xH:'+temp[1]);
  console.log('yL:'+temp[2]);
  console.log('yH:'+temp[3]);
  console.log('zL:'+temp[4]);
  console.log('zH:'+temp[5]);
}

var t = setInterval(getVal, 2000);
```







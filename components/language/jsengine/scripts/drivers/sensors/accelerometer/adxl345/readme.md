# ADXL345

###Driver功能:加速度传感器

adxl345是一款具有数字加速度计功能低功耗传感器，分辨率可以达到13bit，测量范围在16g,通过I2C或SPI协议进行数据交互.


###硬件资源

1.接线

ADXL345模块的VCC连接DevelopKit的VCC引脚；

ADXL345模块的GND连接DevelopKit的GND引脚；

ADXL345模块的SCL连接DevelopKit的SCL引脚；

ADXL345模块的SDA连接DevelopKit的SDA引脚；


###Driver配置

DevelopKit示范：

```json
{
    "adxl345":{
      "type":"I2C",
      "port":2,
      "address_width":7,
      "freq":200000,
      "mode":1,
      "dev_addr":166
    }
}

```

###API说明

```
class：adxl345
param：@id  i2c端口对应的配置ID，对应board.json中的adxl345;
method:
      getAcc() 获取加速度raw数据;

```


###示例
读取加速度raw数据:

```js
var adxl345 = require('adxl345/src/index.js');
var handle = new adxl345('adxl345');
var adxl345_val=[0,0,0];
var t1 = setInterval(function() {
   
    var accVal = handle.getAcc();
    if((adxl345_val[0]==accVal[0])&&(adxl345_val[1]==accVal[1])&&(adxl345_val[2]==accVal[2])){
        return; 
    }
    console.log('xacc:'+accVal[0] + 'yacc:'+accVal[1] + 'zacc:'+accVal[2]);
    adxl345_val[0] = accVal[0];
    adxl345_val[1] = accVal[1];
    adxl345_val[2] = accVal[2];
}, 1000);
```



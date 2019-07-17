# BMP280

###Driver功能： 气压和温度传感器

bmp280是一款用了测量气压值和温度值的传感器，它具有较高的精确度，通过I2C或SPI协议进行数据交互。



###硬件资源

DevelopKit开发板上自带有bmp280传感器：


###Driver配置
developkit示范：

```
{
    "bmp280":{
      "type":"I2C",
      "port":2,
      "address_width":7,
      "freq":400000,
      "mode":1,
      "dev_addr":238
    }
}

```

###API说明

```
class：bmp280
param：@id  i2c端口对应的配置ID，对应board.json中的bmp280;
method:
      getPressure() 获取气压raw值;
      getTemperature() 获取温度raw值;

```




###示例

读取气压和温度的raw数据：

```
var bmp280 = require('bmp280/src/index.js');
var handle = new bmp280('bmp280');
var t1 = setInterval(function() {
   
    var pressure = handle.getPressure();
    var temperature = handle.getTemperature();
    console.log('bmp280 pressure raw data:'+pressure);
    console.log('bmp280 temperature raw data:'+temperature);
}, 3000);
```



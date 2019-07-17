# MAX30100

### Driver功能：血氧和心跳传感器


### 硬件资源

1.MAX30100;

2.Esp32Kit开发板；

### Driver配置

```
{
    "max30100":{
      "type":"I2C",
      "port":1,
      "address_width":7,
      "freq":400000,
      "mode":1,
      "dev_addr":174
    }
}

```


### API说明
```
class：max30100
param：@id  i2c端口对应的id信息，对应board.json中的max30100;

method:
      readData() 读取血氧和心跳raw值;

```


### 示例

```
var MAX30100 = require('max30100/src');
var handle = new MAX30100('max30100');

setInterval(function() {
    var data  = handle.readData();
    if(data[0] < 0 && data[1] < 0){
        return;
    }
    console.log("Blood Oxygen:"+data[0] + " BeatHeat:"+data[1]);
}, 2000);
```
# SHTC1

###Driver功能: 温度湿度传感器SHTC1

shtc1是一款温度&湿度传感器，通过I2C进行数据交互。



###硬件资源

DevelopKit开发板上自带有shtc1传感器：




###Driver配置
developkit 配置示范：

```
{
    "shtc1":{
      "type":"I2C",
      "port":2,
      "address_width":7,
      "freq":100000,
      "mode":1,
      "dev_addr":224
    }
}


```

###API说明
```
class：shtc1
param：@id  i2c端口信息，对应board.json中的shtc1;
method:
      getTempHumi() 获取湿度和温度值;
```

###示例
每隔2秒读取当前环境的温度和湿度:

```
var shtc1 = require('shtc1/src/index.js');
var handle = new shtc1('shtc1');

var shtc1_val=[0,0];
var t1 = setInterval(function() {
   
    var val = handle.getTempHumi();
    if(val[0] == shtc1_val[0] && val[1] == shtc1_val[1]){
        return;
    }
    console.log('shtc1:temp='+val[0]+' humi:'+val[1]);
    shtc1_val[0] = val[0];
    shtc1_val[1] = val[1];
}, 2000);


```
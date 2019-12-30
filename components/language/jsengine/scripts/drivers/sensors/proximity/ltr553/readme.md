# ltr553

###Driver功能：光&距离传感器

ltr553是一款环境光&距离传感器，通过I2C进行数据交互。


###硬件资源

DevelopKit开发板上自带有ltr553传感器：

###Driver配置

```
{
    "ltr553":{
      "type":"I2C",
      "port":1,
      "address_width":7,
      "freq":200000,
      "mode":1,
      "dev_addr":70
    }
}

```


###API说明
```
class：ltr553
param：@id  i2c端口对应的id信息，对应board.json中的ltr553;
method:
      getPsAls() 获取距离值;

```


###示例
每隔2秒读取传感器的数:

```
var ltr553 = require('ltr553/src/index.js');
var handle = new ltr553('ltr553');

var t1 = setInterval(function() {
    var val = handle.getPsAls();
    console.log('ltr553:als='+val[0]+' ps:'+val[1]);
}, 2000);
 
```

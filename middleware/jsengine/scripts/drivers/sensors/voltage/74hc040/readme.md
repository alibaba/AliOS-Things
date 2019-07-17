# 74HC040

### Driver功能：反向器传感器


### 硬件资源

1.74hc040模块;

2.Esp32Kit开发板；

### Driver配置

```
{
    "i74hc.A1":{
        "type":"GPIO",
        "port":18,
        "dir": 0,
        "pull": 1
    }
}


```


### API说明
```
class：I74hcx
param：@id  gpio端口对应的id信息，对应board.json中的mq135.ao;

@do_id  数字量端口对应的id信息，对应board.json中的i74hc.Ax;
method:
      SetVal() 设置电平值;
```


### 示例

```
var I74HC = require('74hc040/src');
var handle = new I74HC('i74hc.A1');
var index = 0;
setInterval(function() {
    index = 1 - index;
    handle.SetVal(index);
}, 2000);
```
# ATGM336H

### Driver功能：gps模块


### 硬件资源

1.atgm336h模块;

2.Esp32Kit开发板；

3.接线

atgm336h模块的TX引脚接Esp32Kit IO16

atgm336h模块的GND引脚接Esp32Kit GND

atgm336h模块的VCC引脚接Esp32Kit 5V
### Driver配置

```
{
    "atgm336h":{
        "type":"UART",
        "port":1,
        "data_width":3,
        "baud_rate":9600,
        "stop_bits":0,
        "flow_control":0,
        "parity_config":0
    }
}


```


### API说明
```
class：atgm336h
param：@id  串口端口对应的id信息，对应board.json中的atgm336h;

method:
      recv() 接收gps信息数据;

```


### 示例

```
var ATGM336H = require('atgm336h/src');
var handle = new ATGM336H('atgm336h');

handle.recv(null,null,function(data){
    console.log('gpsInfo:'+data);
});


```
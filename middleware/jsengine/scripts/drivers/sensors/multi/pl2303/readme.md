# PL2303

### Driver功能：USB转串口模块


### 硬件资源

1.PL2303模块;

2.Esp32Kit开发板；

### Driver配置

```
{
    "pl2303":{
        "type":"UART",
        "port":1,
        "data_width":3,
        "baud_rate":115200,
        "stop_bits":0,
        "flow_control":0,
        "parity_config":0
    }
}


```


### API说明
```
class：pl2303
param：@id  串口端口对应的id信息，对应board.json中的mpl2303;

method:
      recv() 接收数据;
      send() 发送数据;

```


### 示例

```
var PL2303 = require('pl2303/src');
var handle = new PL2303('pl2303');

handle.recv(null,null,function(data){
    console.log('recv:'+data);
});

setInterval(function() {
    handle.send('test send uart data\r\n');
}, 2000);

```
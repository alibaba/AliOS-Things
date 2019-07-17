# XW3

### Driver功能：碰撞检测传感器


### 硬件资源

1.xw3模块;

2.Esp32Kit开发板；

3.接线

xw3模块的DO引脚接Esp32Kit IO18

xw3模块的GND引脚接Esp32Kit GND

xw3模块的VCC引脚接Esp32Kit 3.3V
### Driver配置

```
{
    "xw3":{
        "type":"GPIO",
        "port":18,
        "dir":1,
        "pull":1
    }
}

```


### API说明
```
class：xw3
param：@id  gpio端口对应的id信息，对应board.json中的xw3;

method:
      OnCollision() 监控碰撞;

```


### 示例

实时监控碰撞(测试的时候需要手动触发传感器上的碰撞开关)：

```
var XW3 = require('xw3/src');
var handle = new XW3('xw3');

handle.OnCollision(function(val){
    console.log("Collision!");
});


```
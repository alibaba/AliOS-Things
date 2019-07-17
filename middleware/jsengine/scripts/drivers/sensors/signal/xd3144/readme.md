# XD3144

### Driver功能：霍尔开关/测速模块




### 硬件资源

1.xd3144模块;

2.Esp32Kit开发板；

3.接线

xd3144模块的OUT引脚接Esp32Kit IO18

xd3144模块的GND引脚接Esp32Kit GND

xd3144模块的VCC引脚接Esp32Kit 3.3V
### Driver配置

```
{
    "xd3144":{
        "type":"GPIO",
        "port":18,
        "dir":2,
        "pull":1
    }
}

```


### API说明
```
class：xd3144
param：@id  gpio端口对应的id信息，对应board.json中的xd3144;

method:
      getSpeed() 计算当前speed(/50ms);

```


### 示例

该驱动是每50ms统计一次计数，实际测试的时候需要把传感器靠近磁场较强的地方。
```
var XD3144 = require('xd3144/src');
var handle = new XD3144('xd3144');
handle.getSpeed(function(val){
    console.log("Speed:"+val);
});

```
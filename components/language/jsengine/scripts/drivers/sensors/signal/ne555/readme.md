# NE555

### Driver功能：脉冲发生器

NE555是一款脉冲发生器，其频率可调。


### 硬件资源

1.ne555模块;

2.Esp32Kit开发板；

3.接线

ne555模块的OUT引脚接Esp32Kit IO16

ne555模块的GND引脚接Esp32Kit GND

ne555模块的VCC引脚接Esp32Kit 3.3V
### Driver配置

```
{
    "ne555":{
        "type":"GPIO",
        "port":16,
        "dir":2,
        "pull":1
    }
}

```


### API说明
```
class：ne555
param：@id  gpio端口对应的id信息，对应board.json中的ne555;

method:
      getHZ() 计算当前脉冲频率值;

```


### 示例

```
var NE555 = require('ne555/src');
var handle = new NE555('ne555');
handle.getHz(function(count){
    console.log('hz:'+count + 'n/s');
});

```
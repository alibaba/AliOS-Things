# FC51

### Driver功能：红外避障


### 硬件资源

1.红外避障模块;

2.Esp32Kit开发板；

### Driver配置

```
{
    "fc51":{
        "type":"GPIO",
        "port":18,
        "dir": 1,
        "pull": 1
    }
}

```


### API说明
```
class：fc51
param：@id  gpio端口对应的id信息，对应board.json中的fc51;

method:
      GetFlags() 检测是否有障碍物;

```


### 示例

实时监测是否有障碍物:

```
var FC51 = require('fc51/src');
var handle = new FC51('fc51');

setInterval(function() {
    tmp_val = handle.GetFlags();
    if(0 == tmp_val){
        console.log('Warning! Obstacles..')
    }
}, 100);
```
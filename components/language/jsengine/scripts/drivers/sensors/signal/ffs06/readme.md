# FFS06

### Driver功能：火焰传感器


### 硬件资源

1.FFS06模块;

2.Esp32Kit开发板；

### Driver配置

```
{
    "ffs06.do":{
        "type":"GPIO",
        "port":18,
        "dir": 1,
        "pull": 1
    }
}

```


### API说明
```


class：ffs06
param：@id  gpio端口对应的id信息，对应board.json中的ffs06.do;


method:
      OnFlame() 火情监测;


```


### 示例

实时监测环境火情:
```
var FFS06 = require('ffs06/src');
var handle = new FFS06('ffs06.do');

handle.OnFlame(function(flag){
    console.log('Flame!');
});
```
# HR202

### Driver功能：环境湿度传感器


### 硬件资源

1.HR202模块;

2.Esp32Kit开发板；

3.接线:

HR202模块的VCC引脚接Esp32Kit模块的3.3V引脚;

HR202模块的GND引脚接Esp32Kit模块的GND引脚;

HR202模块的DO引脚接Esp32Kit模块的IO18引脚;


### Driver配置

```
{
    "hr202":{
        "type":"GPIO",
        "port":18,
        "dir": 1,
        "pull": 1
    }
}

```


### API说明
```


class：hr202
param：@id  gpio端口对应的id信息，对应board.json中的hr202;


method:
      OnHum() 湿度监测;


```


### 示例

实时监测环境湿度:
```
var HR202 = require('hr202/src');
var handle = new HR202('hr202');

handle.OnHum(function(flag){
    console.log('Hum!');
});
```
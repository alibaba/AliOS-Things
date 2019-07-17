# ROTEKEY

### Driver功能：旋转按钮


### 硬件资源

1.rotekey模块;

2.Esp32Kit开发板；

3.接线

rotekey模块的clk引脚接Esp32Kit IO16

rotekey模块的sw引脚接Esp32Kit IO18

rotekey模块的dt引脚接Esp32Kit IO19

rotekey模块的GND引脚接Esp32Kit GND

rotekey模块的VCC引脚接Esp32Kit 3.3V
### Driver配置

```
{
    "rotekey.sw":{
        "type":"GPIO",
        "port":18,
        "dir":2,
        "pull":1
    },
    "rotekey.dt":{
        "type":"GPIO",
        "port":19,
        "dir":1,
        "pull":1
    },
    "rotekey.clk":{
        "type":"GPIO",
        "port":16,
        "dir":2,
        "pull":1
    }
}

```



### API说明
```
class：rotekey
param：@swId  sw引脚对应的id信息，对应board.json中的rotekey.sw;

       @dtId  dt引脚对应的id信息，对应board.json中的rotekey.dt;

       @clkId  clk引脚对应的id信息，对应board.json中的rotekey.clk;

method:
      OnPress() 监控中间按钮;
      OnDir()监控旋转方向;

```


### 示例

实时监控按钮的按压和旋转方向(实际测试误差较大，待修正!)：

```
var ROTEKEY = require('rotekey/src');
var handle = new ROTEKEY('rotekey.sw','rotekey.dt','rotekey.clk');

handle.OnPress(function(val){
    console.log('perss it!');
});


handle.OnDir(function(val){
    console.log('val:'+val);
});

```
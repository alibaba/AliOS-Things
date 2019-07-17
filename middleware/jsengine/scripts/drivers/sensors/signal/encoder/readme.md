# encoder



### Driver功能: 测速编码器


### 硬件资源

1.esp32Kit开发板; 

2.encoder模块;

3.接线

encoder模块的left0引脚接Esp32Kit的IO26引脚

encoder模块的left1引脚接Esp32Kit的IO27引脚

encoder模块的right0引脚接Esp32Kit的IO13引脚

encoder模块的right1引脚接Esp32Kit的IO14引脚


### Driver配置

esp32Kit示范:

```
{
  "encoder.left0":{
    "type":"GPIO",
    "port":26,
    "dir":2,
    "pull":1
  },
  "encoder.left1":{
    "type":"GPIO",
    "port":27,
    "dir":1,
    "pull":1
  },
  "encoder.right0":{
    "type":"GPIO",
    "port":13,
    "dir":2,
    "pull":1
  },
  "encoder.right1":{
    "type":"GPIO",
    "port":14,
    "dir":1,
    "pull":1
  }
}

```


###API说明
```


class：encoder
param：@irq_id  引脚配置信息，对应board.json中的encoder.left0、encoder.left1、encoder.right0或encoder.right1


method:
      start() 启动;
      getSpeed() 获取当前编码计数;
      reverseOrForward() 获取正转/反转状态;

```


### 示例

JavaScript

```
var encoder = require("encoder/src/index.js"); 
var left_encoder = new encoder('encoder.left0', 'encoder.left1'); 
var right_encoder = new encoder('encoder.right0', 'encoder.right1'); 

left_encoder.config(500); 
right_encoder.config(500); 

var leftEncoderCb = function(val) {
    console.log('leftEncoderCb:'+val);
};


var rightEncoderCb = function(val) {
    console.log('rightEncoderCb:'+val);
};

left_encoder.start(leftEncoderCb); 
right_encoder.start(rightEncoderCb); 


```
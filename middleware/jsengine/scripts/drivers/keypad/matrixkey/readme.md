#matrixkeypad

###Driver功能：矩阵键盘


###硬件资源

1.ESP32模组

2.矩阵键盘模块

3.接线

矩阵键盘模块C1 接ESP32 16 引脚；

矩阵键盘模块C2 接ESP32 4 引脚；

矩阵键盘模块C3 接ESP32 2 引脚；

矩阵键盘模块C4 接ESP32 15 引脚；

矩阵键盘模块R1 接ESP32 17 引脚；

矩阵键盘模块R2 接ESP32 5 引脚；

矩阵键盘模块R3 接ESP32 18 引脚；

矩阵键盘模块R4 接ESP32 19 引脚；

###Driver配置
esp32Kit示范：

```
{
    "matrixkeypad.C4":{
      "type":"GPIO",
      "port":15,
      "dir":1,
      "pull":1
    },
    "matrixkeypad.C3":{
      "type":"GPIO",
      "port":2,
      "dir":1,
      "pull":1
    },
    "matrixkeypad.C2":{
      "type":"GPIO",
      "port":4,
      "dir":1,
      "pull":1
    },
    "matrixkeypad.C1":{
      "type":"GPIO",
      "port":16,
      "dir":1,
      "pull":1
    },
    "matrixkeypad.R1":{
      "type":"GPIO",
      "port":17,
      "dir":0,
      "pull":1
    },
    "matrixkeypad.R2":{
      "type":"GPIO",
      "port":5,
      "dir":0,
      "pull":1
    },
    "matrixkeypad.R3":{
      "type":"GPIO",
      "port":18,
      "dir":0,
      "pull":1
    },
    "matrixkeypad.R4":{
      "type":"GPIO",
      "port":19,
      "dir":0,
      "pull":1
    }
}

```

###API说明

```
class：matrixkeypad
param：@c1_id  第一列引脚ID，对应board.json中的matrixkeypad.C1;
       @c2_id  第二列引脚ID，对应board.json中的matrixkeypad.C2;
       @c3_id  第三列引脚ID，对应board.json中的matrixkeypad.C3;
       @c4_id  第四列引脚ID，对应board.json中的matrixkeypad.C4;
       @r1_id  第一行引脚ID，对应board.json中的matrixkeypad.R1;
       @r2_id  第二行引脚ID，对应board.json中的matrixkeypad.R2;
       @r3_id  第三行引脚ID，对应board.json中的matrixkeypad.R3;
       @r4_id  第四行引脚ID，对应board.json中的matrixkeypad.R4;

method:
      init() 初始化并注册按键回调函数;
      scan() 按键矩阵扫描;

```



## #示例
按下任意按键，终端将输出对应的行列值:

```
var matrixkeypad = require('./src/matrixkey/src/index.js');
var handle = new matrixkeypad('matrixkeypad.C1','matrixkeypad.C2','matrixkeypad.C3','matrixkeypad.C4','matrixkeypad.R1','matrixkeypad.R2','matrixkeypad.R3','matrixkeypad.R4');
var keycb = function(cval,rval){
    console.log('cval: '+cval + 'rval: '+rval);
};
handle.init(keycb);
setInterval(function(){
   handle.scan();
}, 100);

```



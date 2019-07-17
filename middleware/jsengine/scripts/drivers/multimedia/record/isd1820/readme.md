# ISD1820

###Driver功能: 录音语音模块
该模块是10s录音的语音模块，能进行高质量语音还原。


###硬件资源

1.Esp32开发板;

2.ISD1820模块;

3.接线

isd1820模块 REC引脚接ESP32 19引脚；

isd1820模块 P-E引脚接ESP32 18引脚；

isd1820模块 VCC引脚接ESP32 VCC引脚；

isd1820模块 GND引脚接ESP32 GND引脚；



###Driver配置

```
{
    "isd1820.play":{
      "type":"GPIO",
      "port":18,
      "dir":0,
      "pull":1
    },
    "isd1820.record":{
      "type":"GPIO",
      "port":19,
      "dir":0,
      "pull":1
    }
}
```

###API说明
```
class：isd1820
param：@play_id  play引脚对应的id，对应board.json中的isd1820.play;
       @record_id  record引脚对应的id，对应board.json中的isd1820.record;
method:
      startRecord() 开始录音。
      stopRecord() 停止录音。
      startPlay() 开始播放。
```

###示例
循环的录音、播放:

```
var isd1820 = require('isd1820/src/index.js');
var handle = new isd1820('isd1820.play','isd1820.record')
var  index = 0;
setInterval(function(){

    if(0 == index){
        console.log('start record!');
        handle.startRecord();
    }else if(1 == index){
        console.log('start play!');
        handle.stopRecord();
        handle.startPlay();
    }
    index = index + 1;
    if(2 == index){
        index = 0;
    } 
}, 15000);

```







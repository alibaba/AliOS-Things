# CZN15E

###Driver功能: 声音传感器
czn15e是一种传声器，它能对周围环境的声音进行检测，当声音达到一定阀值时，czn15e模块将输出低电平，否则将维持高电平。


###硬件资源

1.esp32Kit开发板;

2.czn15e模块;

3.接线

czn15e模块VCC引脚接esp32Kit 3V引脚；

czn15e模块GND引脚接esp32Kit GND引脚；

czn15e模块OUT引脚接esp32Kit IO5引脚；

###Driver配置

```json
{
    "czn15e":{
      "type":"GPIO",
      "port":5,
      "dir": 1,
      "pull": 1
    }
}


```

###API说明
```
class：czn15e
param：@id  out引脚对应的配置ID，对应board.json中的czn15e;

method:
      get() 获取out引脚的状态值;

```


###示例

每隔1秒采集czn15e传感器的值，如果周围环境有声音，则打印Sound：YES：

```js
var czn15e = require('czn15e/src/index.js');
var handle  = new czn15e('czn15e');
var t1 = setInterval(function() {
    var val;
    val = handle.get();
    if(1 == val){
        console.log("sound:NO");
    }else{
        console.log("sound:YES");
    }
}, 1000);


```


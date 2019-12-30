# TouchKey

Driver功能：触摸按键


###硬件连接

![](./touch-key.jpeg)


我们使用ESP32DEVKITC开发板的板载触摸按键做示例。如下：


###Driver配置
基于ESP32DEVKITC的Driver配置如下，如其他平台请根据实际连接情况调整Driver的配置。

```json
{
  "ADC":[
		{
		"id":"touchkey.key0",
		"port":4,
		"sampling":12000000
        },
		{
		"id":"touchkey.key1",
		"port":2,
		"sampling":12000000
		}
	]
}
```

###API说明
```
class：touchkey
param：@tkey0_id  第一个触摸按键ID，对应board.json中的touchkey.key0;
       @tkey0_id  第二个触摸按键ID，对应board.json中的touchkey.key1;
method:
      on() 实时监听触摸按键;
```

###示例
实时监测触摸按键是否按下：

```js
var touchkey = require('tkey/src/index.js');
var handle = new touchkey('touchkey.key0','touchkey.key1')
function keyHandle(val){
    console.log('Touch key'+' '+val + 'press');
}
handle.on(keyHandle);

```

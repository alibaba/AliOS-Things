# RGBLED

Driver功能：三色灯

rgbled是一种三色led模块，可以通过对相应引脚的控制让rgbled模块显示不同的颜色。

硬件连接


1.esp32Kit开发板;
2.RGBLED模块;


3.接线
![](rgbled.png)


使用开发板上自带的三色灯模块。
Driver配置
基于ESP32开发板的自带三色灯模块的GPIO引脚配置如下：

```

    "rgbled.r":{
		"type":"GPIO",
		"port":27,
		"dir":3,
		"pull":1
    },
    "rgbled.g":{
		"type":"GPIO",
		"port":32,
		"dir":3,
		"pull":1
    },
    "rgbled.b":{
		"type":"GPIO",
		"port":33,
		"dir":3,
		"pull":1
    }
```
## # API 说明：
	类:
	  rgbled
	     
	构造函数：
	
	  rgbled（red_led_id,green_led_id,blue_led_id）
		  red_led_id    : 红灯ID，对应的board.json 中 rgbled.r
		  green_led_id  : 绿灯ID，对应的board.json 中 rgbled.g
		  blue_led_id   : 蓝灯ID，对应的board.json 中 rgbled.b
		
	函数：
	
	 red()    :闪红灯；
	 blue()   :闪蓝灯；
	 green()  :闪绿灯；
	    

## 示例：

	var rbgled = require('rgbled/src/index.js');
	var iRGB = new rbgled('rgbled.r','rgbled.g','rgbled.b');
	iRGB.blue();
	    

```
该应用实现了三色灯闪烁灯效果。
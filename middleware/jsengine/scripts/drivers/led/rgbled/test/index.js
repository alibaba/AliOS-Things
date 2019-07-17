
//导入RGB 驱动程序模块
var rbgled = require('rgbled/src/index.js');

var i = 0;
var blinkInterval = 3000;  //每3秒

//创建一个新的RGB设备对象
var iRGB = new rbgled('rgbled.r','rgbled.g','rgbled.b');

console.log("===========闪灯演示程序============");
 
//设置周期性交叉点亮各色灯
setInterval(function() {
	
	switch(i)
	{
		case 0:
			iRGB.red();
			console.log("点亮红灯");
			break;
		
		case 1:
			iRGB.blue();
			console.log("点亮蓝灯");
			 break;
		
		case 2:
			iRGB.green();
			console.log("点亮绿灯");
			 break;
		
		default:
				break;
	}
	
	i=(i+1)%3;
	
  }, blinkInterval);
  














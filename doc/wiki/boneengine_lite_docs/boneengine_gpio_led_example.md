# 使用BoneEngine@Lite快速实现LED闪灯的例子

本文介绍如何基于AliOS-Things + BoneEngine@Lite实现一个GPIO控制LED闪灯的例子。



## 1 准备工作：

* 下载AliOS-Things的代码。
* 一款支持BoneEngine@Lite的硬件/模组。如developerkit，mk3060，esp32devkitc。

### 1.1 编译并烧录BoneEngine@Lite固件

* 说明：这里以硬件AliOS-Things developerkit开发板为例。

* 编译：进入AliOS-Things的根目录，执行```aos make tinyengine_app@developerkit```

* 烧录： 编译成功后，通过usb连接developerkit开发板和PC，执行```aos upload tinyengine_app@developerkit```

  

### 1.2  安装be cli工具

be_cli工具是BoneEngine@Lite用于调试js代码的工具。

* MAC/Ubuntu系统：

  进入aos/framework/tinyengine/tools目录，执行```make cli ```命令。

* Windows系统：

  参考文档 [如何在windows下准备be-cli应用开发环境.md](be-cli-for-windows-install)



## 2 基于BoneEngine@Lite实现控制LED的js代码



### 2.1 初始化一个BoneEngine@Lite工程

在PC主机的任意目录执行``be -i bone``,其中bone名字可以任意取，仅代表工程目录名。



### 2.2 配置board_config.json 硬件配置文件

根据硬件版型配置需要控制的gpio口和定义对象ID（led）。例如developerkit板子的gpio 67连接了一个led，因此这里port 为67。

```"GPIO":[
"GPIO":[
		{
			"id":"led",
			"port":67,
			"dir":0,
			"pull":1
		},
	],
```



### 2.3 编辑index.js文件编写js代码

打开bone目录下的index.js文件并实现LED闪烁的js代码，默认情况下，BoneEngine启动后会自动加载index.js文件并运行。

代码如下：

```javascript
console.log('This a demo javascript for bone-engine\n');
console.log('this demo run on developerkit to blink led every second');

var index = 0;
var led_handle = GPIO.open('led');
GPIO.write(led_handle,0);
setInterval(function() {
	index = 1-index;
	GPIO.write(led_handle,index);
	console.log('led blink every seconds...............');
}, 1000);
console.log('end gpio test........................');
```



## 3 推送js文件到硬件/模组上运行



### 3.1使用be_cli打包工程目录 

在工程目录的同级目录执行命令 ```be -p bone```

执行成功会生成一个app.bin文件。



### 3.2 使用be_cli推送app.bin文件到硬件/模组

通过usb连接硬件/模组到pc ，

* 执行命令选择usb端口： ```be connect```

  ```
   be connect
  ------[0] uart: /dev/tty.usbmodem14113
  input number to select device to connect:0
  device has been connected!
  ```

* 执行命令：```be push app.bin```



### 3.3 重启硬件/模组查看js运行效果 

可以看到developerkit的LED2每隔一秒闪烁。


## demo-linkdevelop 设备对接 LD 示例
本示例旨在为开发者展示如何让设备快速与阿里云端的 LD（Link Develop）平台相连，把设备端属性上报到云端展示，手机APP，或者云端远程更新设备的属性和状态，操控设备。

### 在LD平台上，创建一个新的产品和调试设备

*  首先在LD上 [https://linkdevelop.aliyun.com]() 创建一个GAVIN-GUO-LIGHT 的产品

![image | left](../../../docs/graph/LD控制台.jpg "")

* 然后，定义产品GAVIN-GUO-LIGHT 的产品设备模型：属性，事件和方法
 
本示例产品只定义一个属性，即：

```
LightSwitch：开：1，关：0
```
	
* 为该产品生成一个具体设备，设备三元组为：

```
productKey: "xxxxxx",
deviceName: "xxxxxx",
deviceSecret: "XXXXXXX",
```
### 利用 WebIDE 开发工具开发应用程序

如何使用 WebIDE 请参见《WebIDE使用指南》 

* 导入 cloud-clinet 软件模块
* 导入 rgbled 驱动
* 编写主程序index.js

```js
//导入上云模块
var cloudClient = require("cloud-client/src/index.js");

//导入参考板上的 RGB LED 驱动
var rbgled = require("rgbled/src/index.js");

//创建一个RGB 实例
var iRGB = new rbgled("rgbled.r", "rgbled.g", "rgbled.b");
var i;

console.log("=================================");
console.log("       上云测试程序      ");
console.log("=================================");

//开灯
function lightOn() {
  console.log("开灯");

  //rbgled.set(r,g,b) 设置RGB灯颜色，r=0,g=0,b=0 白色
  iRGB.set(0, 0, 0);

  //上报板上的灯的状态属性到云端：开灯
  cloudClient.postProperty({ LightSwitch: 1 });
}

//关灯
function lightOff() {
  console.log("关灯");
  //rbgled.set(r,g,b) 设置RGB灯颜色，r=1,g=1,b=1 关闭灯
  iRGB.set(1, 1, 1);
  //上报板上的灯的状态属性到云端：关闭
  cloudClient.postProperty({ LightSwitch: 0 });
}

//启动上云模块
cloudClient.start(
  //config配置参数
  {
    //设备三元组，LD以电子书测试账号002登陆，荆懋测试项目，GAVIN_GUO_LIGHT 设备
    productKey: "xxxxxx",
    deviceName: "xxxxxxx",
    deviceSecret: "xxxxxxxxxxxxxxxxxxxxx"
  },

  //回调函数cb
  function(err, data) {
    console.log("onStart");
    if (err) {
      return;
    }
    //注册响应云端事件的回调函数
    cloudClient.onPropertySet(function(msg) {
      //云端下发的设置命令
      if (msg.method === "thing.service.property.set") {
        if (msg.params.LightSwitch === 1) {
          //开灯
          lightOn();
        } else {
          //关灯
          lightOff();
        }
      }
    });
  }
);

//定时开关灯，10秒
setInterval(function() {
  if (i) lightOn();
  else lightOff();

  i = (i + 1) % 2;
}, 10000);
```
### 调试硬件
调试设备选用 ESP32 开发板 goouuu-esp-32f ，烧录TinyEngine官方固件，并设置 WIFI 网络

![image | left](../../../docs/graph/ESP Goouuu 开发板.jpg "")

board.json  配置如下：

```json
{
  "rgbled.r": {
    "type": "GPIO",
    "port": 27,
    "dir": 3,
    "pull": 1
  },
  "rgbled.g": {
    "type": "GPIO",
    "port": 33,
    "dir": 3,
    "pull": 1
  },
  "rgbled.b": {
    "type": "GPIO",
    "port": 32,
    "dir": 3,
    "pull": 1
  }
}

```
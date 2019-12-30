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
    deviceName: "xxxxxx",
    deviceSecret: "xxxxxx"
  },

  //回调函数cb
  function (err, data) {
    console.log("onStart");
    if (err) {
      return;
    }
    //注册响应云端事件的回调函数
    cloudClient.onPropertySet(function (msg) {
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
setInterval(function () {
  if (i) lightOn();
  else lightOff();

  i = (i + 1) % 2;
}, 10000);

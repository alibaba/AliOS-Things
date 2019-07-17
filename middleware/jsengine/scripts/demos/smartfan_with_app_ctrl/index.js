var hxd019 = require("hxd019");
var IRcode = require("ir-code/src/index.js");
var deviceShadow = require("device-shadow/src/index.js");

console.log("=================================");
console.log("             demo-fan            ");
console.log("=================================");

//设备三元组
//填入在云端注册的设备三要素
var device = {
  productKey: "xxxxxx",
  deviceName: "xxxxxx",
  deviceSecret: "xxxxxx",
  domain: "xx.xx.xx.xx",
  port: 1883
};

//创建红外遥控器对象
var hxdHandle = new hxd019("hxd019.scl", "hxd019.sda", "hxd019.busy");

// 初始化 deviceShadow
function initDeviceShadow() {
  deviceShadow.bindDevID(device);
  deviceShadow.start(deviceShadowInitCB);
}

// 设备初始化的回调函数，在初始化成功时，会注册云端属性控制函数
function deviceShadowInitCB(err) {
  if (err) {
    console.log("连接 LD 失败, err = " + err);
    // 再次重连
    setTimeout(initDeviceShadow, 2000);
  } else {
    console.log(" 连接 LD 成功 ");
    registAllCloudNotify();
  }
}

// 注册所有云端控制设备端属性的处理函数
function registAllCloudNotify() {
  deviceShadow.addDevSetPropertyNotify("powerFanOnOrOff", powerFanOnOrOff);
  deviceShadow.addDevSetPropertyNotify("weakFanWind", weakFanWind);
  deviceShadow.addDevSetPropertyNotify("strongFanWind", strongFanWind);
  deviceShadow.addDevSetPropertyNotify("swingFanLOrR", swingFanLeftOrRight);
  deviceShadow.addDevSetPropertyNotify("swingFanUOrD", swingFanUpOrDown);
}

// 风扇 开/关
function powerFanOnOrOff(param) {
  hxdHandle.send(IRcode.Switch);
  console.log("let fan on or off");
}

// 减弱风速
function weakFanWind(param) {
  hxdHandle.send(IRcode.LowLevel);
  console.log("减弱风扇风速");
}

// 加大风速
function strongFanWind(param) {
  hxdHandle.send(IRcode.HighLevel);
  console.log("加大风扇风速");
}

// 左右摆动
function swingFanLeftOrRight(param) {
  hxdHandle.send(IRcode.LRShake);
  console.log("风扇左右摆动");
}

// 上下摆动
function swingFanUpOrDown(param) {
  hxdHandle.send(IRcode.UDShake);
  console.log("风扇上下摆动");
}

// 周期行判断网络是否准备好，如果准备好，则连接LD，并注册本地处理函数
var t1 = setInterval(function() {
  //每秒检测一次网卡是否连上网络？
  var ip = WIFI.getip();
  if (ip != null) {
    clearInterval(t1);
    process.nextTick(initDeviceShadow);
  }
}, 1000);

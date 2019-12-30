console.log("=================================");
console.log("             demo-fan            ");
console.log("=================================");

var hxd019 = require("hxd019/src/index.js");
var IRcode = require("IRcode/src/index.js");
var hxdHandle = new hxd019("hxd019.scl", "hxd019.sda", "hxd019.busy");

// 风扇 开/关
function fanOnOrOff() {
  hxdHandle.send(IRcode.Switch);
  console.log("let fan on or off");
}

// 风扇 弱风
function fanLow() {
  hxdHandle.send(IRcode.LowLevel);
  console.log("let fan slow");
}

// 风扇 强风
function fanHigh() {
  hxdHandle.send(IRcode.HighLevel);
  console.log("let fan faster");
}

// 风扇 左右摆动
function fanLR() {
  hxdHandle.send(IRcode.LRShake);
  console.log("let fan left and right shake");
}

// 风扇 上下摆动
function fanUD() {
  hxdHandle.send(IRcode.UDShake);
  console.log("let fan up and down shake");
}

setInterval(function () {
  console.log("fun is running");
}, 5000);

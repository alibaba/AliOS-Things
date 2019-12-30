/**
 * 设备与云端交互测试
 */
var deviceShadow = require("../src/index.js");

console.log("=================================");
console.log("        deviceShadow test       ");
console.log("=================================");

function initDeviceShadow() {
  //设备三元组
  var config = {
    productKey: 'please_input_your_key',
    deviceName: 'please_input_device_name',
    deviceSecret: 'please_input_device_secret',
    domain: 'iot-as-mqtt.cn-shanghai.aliyuncs.com',
    port: 1883
  };
  deviceShadow.bindDevID(config);
  deviceShadow.start(deviceShadowStartCB);
}

// 设备初始化回调函数
function deviceShadowStartCB(err) {
  if (err) {
    console.log('连接 LD 失败, err = ' + err);
    // 再次重连
    setTimeout(initDeviceShadow, 2000);
  } else {
    console.log('连接 LD 成功 ');
    registAllCloudNotify();

    console.log('上报属性');
    deviceShadow.postProperty('BatteryPercentage', 55);

    console.log('上报事件');
    deviceShadow.postEvent('Error', {
      ErrorCode: 0
    });
    deviceShadow.postEvent('Error', {
      ErrorCode: 1
    });
  }
}

// 注册所有云端设置设备端属性的处理函数
function registAllCloudNotify() {
  deviceShadow.addDevSetPropertyNotify('batteryAlarm', cloudSetProperty);
  deviceShadow.addDevCallServiceNotify('Reboot', cloudCallService);
}

// 设备端本地处理函数，将会注册到云端调用
function cloudSetProperty(val) {
  console.log('云端设置batteryAlarm=' + val);
}

function cloudCallService(msg, topic) {
  console.log('云端调用CallService, topic=' + topic);
  var val = JSON.stringify(msg);
  console.log('msg = ' + val);
}

var t1 = setInterval(function () {

  //每秒检测一次网卡是否连上网络？
  var ip = WIFI.getip();
  if (ip != null) {
    clearInterval(t1);

    //连云之前也可以post,设备信息会缓存,等待上云连接成功之后会上报
    // deviceShadow.postProperty('BatteryPercentage', 40);

    console.log('开始链接 ' + 'LD Platform');

    process.nextTick(initDeviceShadow);
  }
}, 1000);
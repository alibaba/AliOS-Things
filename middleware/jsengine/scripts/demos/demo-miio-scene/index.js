console.log("=================================");
console.log("         demo-miio-scene         ");
console.log("=================================");

// miio 为 Natvie Modules，按照小米设备接入协议实现的扩展对象

var lightDevice;

// 控制开灯
function lightOn() {
  console.log("lightOn");
  if (lightDevice) {
    miio.deviceControl(lightDevice, "set_power", '["on"]');
  }
}

// 控制开关
function lightOff() {
  console.log("lightOff");
  if (lightDevice) {
    miio.deviceControl(lightDevice, "set_power", '["off"]');
  }
}

function setupLight(host) {
  // 创建小米灯设备，如何获取小米设备的token，
  //其具体方法和步骤请见：https://lark.alipay.com/tinyenginedev/developer-documentation/wbm6d5
  lightDevice = miio.createDevice(
    host,
    "ae65d127852df5c2b86f38b6878e8706"
  );

  // 初次启动时，第一次关闭小米灯
  lightOff();
}

function setupGateway(host) {
  // 创建小米 Zigbee 网关设备：如何获取小米设备的token，
  //其具体方法和步骤请见：https://lark.alipay.com/tinyenginedev/developer-documentation/wbm6d5
  var gatewayDevice = miio.createDevice(
    host,
    "0a7b7e5af0fc0b1b4877b0907834f9a7"
  );

  var motionSensorId = null;
  var timeout = null;
  // 注册监听事件，回调函数
  miio.deviceOnEvent(gatewayDevice, function (event) {
    console.log("gateway receive event");

    var obj = JSON.parse(event);
    var open = false;

    if (obj.data) {
      open = JSON.parse(obj.data).status === "motion";
    }
    //如果人体感应器检测到人，则开灯，如果一分钟没有感应到人存在，则关灯
    if (obj.cmd === "report" && obj.sid === motionSensorId && open) {
      lightOn();
      if (timeout) {
        clearTimeout(timeout);
        timeout = null;
      }
      //每分钟接收一次
      timeout = setTimeout(function () {
        lightOff();
      }, 60 * 1000);
    }
  });

  // 获取小米网关的设备列表
  var device_list = miio.deviceControl(
    gatewayDevice,
    "get_device_prop",
    '["lumi.0","device_list"]'
  );
  console.log("device list: " + device_list);

  // 从小米网关下挂的设备列表中，找到人体感应设备，网关下的其他设备可以按照类似的方法
  var deviceList = JSON.parse(device_list);
  for (var i = 0; i < deviceList.result.length; i++) {
    if (deviceList.result[i + 1] === 2) {
      motionSensorId = deviceList.result[i].substring(5);
      console.log("motion sensor id: " + motionSensorId);
      break;
    }
  }
}

miio.discover(20, function (host, deviceId) {
  console.log('discovered device, host: ' + host + ', deviceId: ' + deviceId);
  if (deviceId === 61632282) {
    setupLight(host);
  } else if (deviceId === 78463041) {
    setupGateway(host);
  }
});

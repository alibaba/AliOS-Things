console.log("=================================");
console.log("         demo-miio-scene         ");
console.log("=================================");

// miio is JSEngine Natvie Modules

var lightDevice;

// control light
function lightOn() {
  console.log("lightOn");
  if (lightDevice) {
    miio.deviceControl(lightDevice, "set_power", '["on"]');
  }
}

// control light
function lightOff() {
  console.log("lightOff");
  if (lightDevice) {
    miio.deviceControl(lightDevice, "set_power", '["off"]');
  }
}

function setupLight(host) {
  // create XiaoMi device with the token
  lightDevice = miio.createDevice(
    host,
    "ae65d127852df5c2b86f38b6878e8706"
  );

  // turn off the light when startup
  lightOff();
}

function setupGateway(host) {
  // create XiaoMi Zigbee gateway device with the token
  var gatewayDevice = miio.createDevice(
    host,
    "0a7b7e5af0fc0b1b4877b0907834f9a7"
  );

  var motionSensorId = null;
  var timeout = null;
  // register event callback function
  miio.deviceOnEvent(gatewayDevice, function (event) {
    console.log("gateway receive event");

    var obj = JSON.parse(event);
    var open = false;

    if (obj.data) {
      open = JSON.parse(obj.data).status === "motion";
    }
    // turn on the light if detect person
    if (obj.cmd === "report" && obj.sid === motionSensorId && open) {
      lightOn();
      if (timeout) {
        clearTimeout(timeout);
        timeout = null;
      }

      timeout = setTimeout(function () {
        lightOff();
      }, 60 * 1000);
    }
  });

  // get XiaoMi gateway devices
  var device_list = miio.deviceControl(
    gatewayDevice,
    "get_device_prop",
    '["lumi.0","device_list"]'
  );
  console.log("device list: " + device_list);

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

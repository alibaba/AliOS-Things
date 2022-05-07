import * as AIOT_DEVICE from 'AIOT_DEVICE';
import * as fs from 'FS';
import * as kv from 'kv';
import * as i2c from 'i2c'
import * as NETMGR from 'NETMGR';
import * as sht3x from './sht3x.js';
import * as gpio from 'GPIO';

var temperature = 25.0;
var humidity = 0.0;
var airconditioner_value = 0;
var humidifier_value = 0;
var i2cDev = i2c.open({
    id: 'sht3x',
    success: function() {
        console.log('i2c: open success')
    },
    fail: function() {
        console.log('i2c: open failed')
    }
});

var sht3xDev = sht3x.SHT3X(i2cDev);
var led_b = gpio.open('led_b');
var led_g = gpio.open('led_g');

NETMGR.serviceInit();
var dev_handler = NETMGR.getDev('/dev/wifi0');
var device;

/* 1. WIFI配网，连接路由器 */
var keySSID = 'oneMinuteCloudSSID';
var keyPassword = 'oneMinuteCloudPassword';
var wifiSSID = kv.getStorageSync(keySSID);
var wifiPassword = kv.getStorageSync(keyPassword);
console.log('wifi ssid: ' + wifiSSID + ', pswd ' + wifiPassword);
if ((wifiSSID !==undefined) && (wifiPassword !==undefined)) {
  var wifiOptions = {
    ssid: wifiSSID,
    password: wifiPassword,
    bssid: '',
    timeout_ms: 18000
  };

  NETMGR.connect(dev_handler, wifiOptions, function (status) {
    if (status == 'DISCONNECT') {
      console.log('wifi disconnected');
    } else {
      console.log('wifi connected');
      connectAliyunCloud();
    }
  });
}

function connectAliyunCloud() {
  var filePath = '/DeviceInfo.json';
  var data = fs.read(filePath);
  var jsonObj = JSON.parse(data);
  var deviceSecret = jsonObj.deviceSecret;
  var productKey = jsonObj.productKey;
  var deviceName = jsonObj.deviceName;

  var iotOptions = {
    productKey: productKey,
    deviceName: deviceName,
    deviceSecret: deviceSecret,
    region: 'cn-shanghai',
    keepaliveSec: 60
  }
  device = AIOT_DEVICE.device(iotOptions, function(res) {
    if (!res.handle) {
	    console.log("res.handle is empty");
	    return;
    }
    switch (res.code) {
	    case 0:
        console.log('iot platform connect ');
	      device.onProps(function (res) {
	        console.log('cloud req msg_id is ' + res.msg_id);
	        console.log('cloud req params_len is ' + res.params_len);
	        console.log('cloud req params is ' + res.params);
	        var payload = JSON.parse(res.params);
	        if (payload.humidifier !== undefined) {
		        humidifier_value = payload.humidifier;
		        if (humidifier_value == 1) {
		          console.log('打开加湿器');
		        } else {
		          console.log('关闭加湿器');
		        }
		        led_g.write(humidifier_value);
	        }
	        if (payload.airconditioner !== undefined) {
		        airconditioner_value = payload.airconditioner;
		        if (airconditioner_value == 1) {
		          console.log('打开空调');
		        } else {
		          console.log('关闭空调');
		        }
		      led_b.write(airconditioner_value);
	        }

	        /* post props */
	        device.postProps(JSON.stringify({
		        airconditioner: airconditioner_value,
		        humidifier: humidifier_value
	        }));
	      });

	      /* 云端下发服务事件 */
	      device.onService(function (res) {
	        console.log('received cloud msg_id is ' + res.msg_id);
	        console.log('received cloud service_id is ' + res.service_id);
	        console.log('received cloud params_len is ' + res.params_len);
	        console.log('received cloud params is ' + res.params);
	      });

	      setInterval(function() {
          reportEnvironmentData();
	      }, 5000);
        break;
	    case 2:
        console.log('iot platform disconnect ');
        break;
	    default :
        break ;
    }
  });
}

/* 3. 周期上报土壤传感器数据至阿里云物联网平台 */
function reportEnvironmentData() {
  temperature = sht3xDev.getTemperature();
  humidity = sht3xDev.getHumidity();
  temperature = Math.round(temperature * 100) / 100;
  humidity = Math.round(humidity * 100) / 100;
  console.log('Temperature is ' + temperature + ' degrees, humidity is ' + humidity + "\r\n");
  device.postProps(JSON.stringify({
    'CurrentTemperature': temperature,
    'CurrentHumidity': humidity
  }));
}

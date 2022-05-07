import * as AIOT_DEVICE from 'AIOT_DEVICE';
import * as fs from 'FS';
import * as kv from 'kv';
import * as adc from 'ADC';
import * as pwm from 'PWM';
import * as NETMGR from 'NETMGR';

var pwm2 = pwm.open('PWM2');
var adc0 = adc.open('adc0');
var adc7 = adc.open('adc7');

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
    ssid: wifiSSID,          /* 本示例是一分钟上云示例，SSID已经提前写入文件中。非一分钟上云示例，用户需写入SSID */
    password: wifiPassword,  /* 本示例是一分钟上云示例，PSWD已经提前写入文件中。非一分钟上云示例，用户需写入PSWD */
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
	        var duty = payload.pwm;
	        console.log('cloud set pwm2 freq 200hz, duty ' + duty);
	        pwm2.setConfig({
		        freq: 200,
		        duty: duty
	        });

	        device.postProps(JSON.stringify({
		        'pwm': duty
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
	        reportAdcValue();
	      }, 5000);
        break;
	    case 2:
        console.log('iot platform disconnect ');
        break;
	    default : break ;
    }
  });
}

/* 3. 周期上报ADC数据至阿里云物联网平台 */
function reportAdcValue() {
  var Voltage0 = adc0.read();
  var Voltage7 = adc7.read();
  console.log('>> adc0 voltage is ' + Voltage0 + ', adc7 voltage is ' + Voltage7);
  device.postProps(JSON.stringify({
    'adc0': Voltage0,
    'adc3': 0,
    'adc4': 0,
    'adc5': 0,
    'adc6': 0,
    'adc7': Voltage7
  }));
}


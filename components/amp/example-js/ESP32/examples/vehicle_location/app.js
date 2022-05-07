import * as AIOT_DEVICE from 'AIOT_DEVICE';
import * as fs from 'FS';
import * as kv from 'kv';
import * as uart from 'UART';
import * as NETMGR from 'NETMGR';
import * as gps from './gps.js';

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
	      });

	      /* 云端下发服务事件 */
	      device.onService(function (res) {
	        console.log('received cloud msg_id is ' + res.msg_id);
	        console.log('received cloud service_id is ' + res.service_id);
	        console.log('received cloud params_len is ' + res.params_len);
	        console.log('received cloud params is ' + res.params);
	      });
        /* 上报GPS位置信息到阿里云物联网平台显示位置 */
        reportLocationData();
        break;
	    case 2:
        console.log('iot platform disconnect ');
        break;
	    default:
        break ;
    }
  });
}

/* 3. 上报GPS传感器数据至阿里云物联网平台 */
function reportLocationData() {
  var gps_port = uart.open('serial2');
  gps_port.on(function(data, len) {
    var GGA;
    //console.log('gnss data = ' + data)
    var bbb = data.split("$");
    GGA = "$" + bbb[1];
    var gga_tag = GGA.split(",");
    if (gga_tag[0] == '$GNGGA') {
      // console.log('get GGA = ' + GGA)
      var geoLocation_data = gps.ParseData(GGA);
      console.log("reportLocationData: " + JSON.stringify(geoLocation_data, null, 4))
      device.postProps(JSON.stringify({
        GeoLocation: {
          Longitude: geoLocation_data['lon'],
          Latitude: geoLocation_data['lat'],
          Altitude: geoLocation_data['alt'],
          CoordinateSystem: 1
        }
      }), function() {});
    }
  });
}

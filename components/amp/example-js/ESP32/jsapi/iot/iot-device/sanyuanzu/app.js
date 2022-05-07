import * as gpio from 'gpio';
import * as network from 'network';
import * as mqtt from 'mqtt';
import * as iot from 'iot';

/* open GPIO LED1 */
var LED1 = gpio.open({
  id: 'led',
  success: function() {
    console.log('gpio: open LED1 success')
  },
  fail: function() {
    console.log('gpio: open LED1 failed')
  }
});

console.log('guyin: turn off led2');
var value = 1;
LED1.writeValue(value);

setInterval(function() {
  value = 1 - value;
  LED1.writeValue(value);
}, 500);

var net = network.openNetWorkClient({devType: 'wifi'});

/**
 * 获取网络类型
 * 目前支持两种类型：wifi cellular（蜂窝网）
 */
 var netType = net.getType();
 console.log('net type is: ' + netType);

 /**
  * 获取网络状态
  * 目前支持两种状态：connect disconnect
  */
 var netStatus = net.getStatus();
 console.log('net status is: ' + netStatus);

 if (netStatus == 'connect') {
     /* 网络状态已连接，获取网络状态 */
     console.log('net status is connect');
     iotDeviceCreate();
 } else {
     /* wifi或者以太网，设置ifconfig */
      if (netType === "wifi" || netType === "ethnet") {
        console.log('net setIfConfig');
        net.setIfConfig({
          dhcp_en: true,             // true: 动态IP模式(以下参数可忽略）; false: 静态IP模式（以下参数必填）
          ip_addr: '192.168.124.66',
          mask: '255.255.255.0',
          gw: '192.168.124.1',
          dns_server: '8.8.8.8'
        });
      }

     /* 网络状态未连接，如果是wifi设备则主动联网(4G模组中系统会自动注网) */
     if (netType === "wifi") {
         net.connect({
             ssid: 'xxx',                 //请替换为自己的热点ssid
             password: 'xxx'              //请替换为自己热点的密码
         });
     }

     /**
      * 监听网络连接成功事件，成功执行回调函数
      */
     net.on('connect', function () {
         console.log('network connected');
         getNetInfo();
         iotDeviceCreate();
     });

     net.on('disconnect', function () {
      console.log('network disconnected');
  });
 }

 function getNetInfo() {
  console.log('net status is connected, begin getting net information...');
  var info = net.getInfo();
  if (netType === "wifi" || netType === "ethnet") {
    /* 是否开启dhcp */
    console.log('net dhcp_en is: ' + info.netInfo.dhcp_en);

    /* ip地址*/
    console.log('net ip_addr is: ' + info.netInfo.ip_addr);

    /* dns */
    console.log('net dns_server is: ' + info.netInfo.dns_server);

    /* 网关 */
    console.log('net gateway is: ' + info.netInfo.gw);

    /* 子网掩码 */
    console.log('net mask is: ' + info.netInfo.mask);

    /* mac地址 */
    console.log('net mac is: ' + info.netInfo.mac);

    /* wifi信号强度 */
    console.log('net wifi rssi is: ' + info.netInfo.rssi);

    return;
  }
  console.log('unkown net type');
}

var iotdev;
const productKey = 'xxx'; //请替换为自己的pk
const deviceName = 'xxx'; //请替换为自己的dn
const deviceSecret = 'xxx'; //请替换为自己的ds
var topic = '/sys/' + productKey + '/' + deviceName + '/user/haas/info';
var lightSwitch = 0;
var errCode = 0;

function iotDeviceCreate()
{
    iotdev = iot.device({
        productKey: productKey,
        deviceName: deviceName,
        deviceSecret: deviceSecret
    });

    iotdev.on('connect', function () {
        console.log('success connect to aliyun iot server');
        iotDeviceOnConnect();
    });

    iotdev.on('reconnect', function () {
        console.log('success reconnect to aliyun iot server');
    });

    iotdev.on('disconnect', function () {
        console.log('aliyun iot server disconnected');
    });
}

function iotDeviceOnConnect()
{
    iotdev.subscribe({
        topic: topic,
        qos: 0
    });

    iotdev.unsubscribe(topic);

    iotdev.publish({
        topic: topic,
        payload: 'haas amp perfect',
        qos: 1
    });

    setInterval(function() {
        lightSwitch = 1 - lightSwitch;

        // Post properity
        iotdev.postProps(
            JSON.stringify({
                LightSwitch: lightSwitch
            })
        );

        //Post event
        iotdev.postEvent({
            id: 'ErrorMessage',
            params: JSON.stringify({
                ErrorCode: errCode
            })
        });

        iotdev.publish({
            topic: topic,
            payload: 'haas amp perfect',
            qos: 0
        });

    console.log('post lightSwitch ' + lightSwitch + ', ErrorCode ' + errCode);
        console.log('system heapFree: ' + system.memory().heapFree);

    errCode++;
    }, 1000);

    iotdev.onService(function(service) {
        console.log('received cloud service id ' + service.service_id);
        console.log('received cloud service param  ' + service.params);
        console.log('received cloud service param len  ' + service.params_len);
    });

    iotdev.onProps(function(properity) {
        console.log('received cloud properity param ' + properity.params);
        console.log('received cloud properity param len ' + properity.params_len);
    });

}

var kv = require('kv');
var network = require('network');
var iot = require('iot');
var net = network.openNetWorkClient({devType: 'wifi'});
var iotdev;
var lightSwitch = 0;
var errCode = 0;
var ds;
var dynregParams = {
    productKey: 'xx',         // 请替换为自己的product key
    deviceName: 'xx',         // 请替换为自己板子的deviceName，可以用日志中的“amp device name”，也可以自定义，必须和云端已创建的设备名一致
    productSecret:'xx'        // 请替换为自己的product secret
 };
var topic = '/sys/' + dynregParams.productKey + '/' + dynregParams.deviceName + '/user/haas/info';

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

 ds = kv.getStorageSync('_amp_customer_devicesecret');

 if (netStatus == 'connect') {
     /* 网络状态已连接，获取网络状态 */
     console.log('net status is connect');
     if(ds === undefined) {
        console.log('ds is null, dynamic register device ');
        iotDeviceDynreg();
     } else {
        console.log('ds is available, create iot device ');
        iotDeviceCreate();
     }
 } else {
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
         if(ds === undefined) {
            console.log('ds is null, dynamic register device ');
            iotDeviceDynreg();
         } else {
            console.log('ds is available, create iot device ');
            iotDeviceCreate();
         }
     });
     net.on('disconnect', function () {
      console.log('network disconnected');
  });
 }

 function iotDeviceDynreg()
 {
    iot.dynreg(dynregParams, function(data) {
        console.log('rx dynreg DS: ' + data);
        ds = data;
        iotDeviceCreate();
     });
 }

 function iotDeviceCreate()
 {
     iotdev = iot.device({
         productKey: dynregParams.productKey,
         deviceName: dynregParams.deviceName,
         deviceSecret: ds
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
     iotdev.getNtpTime(function(res) {
         console.log('ntp time ' + res.year + '-' + res.month + '-' + res.day + ' ' + res.hour + ':' + res.minute + ':' + res.second + '.' + res.msecond + ', timestamp ' + res.timestamp);
     });

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

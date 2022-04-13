/* iot - 阿里云IoT连接平台连接组件示例，产品功能定义：
 * [功能类型] ------ [功能名称] ------ [标志符] ------ [数据类型]
 *   属性            主灯开关        LightSwitch      bool （0-关闭 1-开启） 
 *   服务            开关翻转     ToggleLightSwitch   -
 *   事件            故障上报           Error         参数标志符：ErrorCode
 * */

var iot = require('iot');

var productKey = 'a10h6IdruJf';
var deviceName = 'nodemcu_02';
var deviceSecret = 'StFYN4UigwDiRbhRwVFkFIKsEqpuaHKu';

var lightSwitch = 0;

var device = iot.device({
      productKey: productKey,
      deviceName: deviceName,
      deviceSecret: deviceSecret,
      region: 'cn-shanghai',
      success: function() {
        console.log('connect success');
        onConnect();
      },
      fail: function() {
        console.log('connect failed');
      }
});

function onConnect() {
  setInterval(function() {
    /** post properties */
    lightSwitch = 1 - lightSwitch;
    device.postProps({
      payload: {
        LightSwitch: lightSwitch
      },
      success: function() {
        console.log('postProps success');
      },
      fail: function() {
        console.log('postProps failed');
      }
    });
    /** post events */
    device.postEvent({
      id: 'Error',
      params: {
        ErrorCode: 0
      },
      success: function() {
        console.log('postEvent success');
        
      },
      fail: function() {
        console.log('postEvent failed');
      }
    });
  }, 3000);
}

device.on('connect', function() {
  console.log('(re)connected');
});

/* 网络断开事件 */
device.on('disconnect', function() {
  console.log('disconnect ');
});

/* 关闭连接事件 */
device.on('close', function() {
  console.log('iot client just closed');
});

/* 发生错误事件 */
device.on('error', function(err) {
  console.log('error ' + err);
});

/* 云端设置属性事件 */
device.on('props', function(payload){
  console.log('cloud req data is ', payload);
  console.log('LightSwitch ' , payload.LightSwitch ? 'ON' : 'OFF');
});

/* 云端下发服务事件 */
device.on('service', function(id, payload){
  console.log('received cloud serviceid is ' + id);
  console.log('received cloud req_data is ' + payload);
});
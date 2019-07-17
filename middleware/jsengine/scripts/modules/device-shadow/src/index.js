// Link Developer

var deviceShadow = {};
//绑定设备三元组
var dev_config = {
  productKey: '',
  deviceName: '',
  deviceSecret: ''
  // type: default 0 , 一机一密
}

var prefix;
var cmdId = 0;

var dev_mqtt_started = 0;

// 设备Shadow
var dev_property = {};
var dev_property_update = {}

var dev_event = {};
var dev_event_update = {}

var dev_start_nofity_fn = null;
var dev_start_nofity_err = undefined;

// 云端下发属性
var cloud_property = {};
var cloud_service = {};

function notify_app_start() {
  if (dev_start_nofity_fn) {
    dev_start_nofity_fn(dev_start_nofity_err);
  }
}


function mqtt_service_set(msg, topic) {
  // console.log('收到set命令, topic=' + topic);
  // var val = JSON.stringify(msg); 
  // console.log('msg = ' + val);

  var params = msg.params;
  var item;

  if (params) {
    // 遍历属性
    var val = JSON.stringify(params);

    console.log('params = ' + val);

    for (item in cloud_property) {
      if (params[item] !== undefined) {

        console.log(item + ' = ' + params[item]);

        var func = cloud_property[item];
        if (func) {
          func(params[item]);
        }
      }
    }
  }

}


// 连接成功之后上报缓存数据
function after_connect_success() {
  var x;
  for (x in dev_property_update) {
    console.log('x = ' + x);
    console.log(dev_property_update[x])
    if (dev_property_update[x]) {
      console.log('上报属性 ' + x);
      deviceShadow.postProperty(x, dev_property[x]);
    }
  }

  for (x in dev_event_update) {
    console.log('x = ' + x);
    console.log(dev_event_update[x])
    if (dev_event_update[x]) {
      console.log('上报事件 ' + x);
      deviceShadow.postEvent(x, dev_property[x]);
    }
  }

  // 订阅云端 set
  MQTT.subscribe(prefix + 'thing/service/property/set', mqtt_service_set);

}

function mqtt_start_notify(err) {
  // console.log('mqtt callback, err = ' + err);

  if(err){
    // 连接失败
    dev_mqtt_started = 0
  }else {
    // 连接正常
    dev_mqtt_started = 2;
    // console.log('连接正常,dev_mqtt_started=' + dev_mqtt_started);
  }

  dev_start_nofity_err = err;
  notify_app_start();

  if (dev_mqtt_started == 2) {
    // 上报之前暂存的信息
    process.nextTick(after_connect_success);
  }
}

deviceShadow.bindDevID = function (config) {
  // console.log('call  deviceShadow.bindDevID');
  dev_config.productKey = config.productKey;
  dev_config.deviceName = config.deviceName;
  dev_config.deviceSecret = config.deviceSecret;
  if(config.domain){
    console.log('config.domain = ' + config.domain);
    dev_config.domain = config.domain;
  }
  if(config.port)
    dev_config.port = config.port;
}

// 云端控制设备
// 注册云端设置属性的回调处理
deviceShadow.addDevSetPropertyNotify = function (name, ntfy_fn) {
  // console.log('call deviceShadow.addDevSetPropertyNotify');
  cloud_property[name] = ntfy_fn;
}

// 注册云端远程调用服务的回调处理
deviceShadow.addDevCallServiceNotify = function (name, ntfy_fn) {
  // console.log('call deviceShadow.addDevCallServiceNotify');
  cloud_service[name] = ntfy_fn;

  if (ntfy_fn) {
    // 订阅
    function mqtt_service_call(msg, topic) {
      // console.log('收到call service命令, topic=' + topic);
      // var val = JSON.stringify(msg);
      // console.log('msg = ' + val);
      if (ntfy_fn)
        ntfy_fn(msg, topic);
    }
    MQTT.subscribe(prefix + 'thing/service/' + name, mqtt_service_call);
  } else {
    // 取消订阅
    MQTT.unsubscribe(prefix + 'thing/service/' + name);
  }
}

// 通知云端
// 主动上报属性
deviceShadow.postProperty = function (name, value) {
  // console.log('call deviceShadow.postProperty');
  dev_property[name] = value;
  dev_property_update[name] = undefined;
  if (dev_mqtt_started == 2) {
    console.log('call MQTT.publish');
    cmdId = cmdId + 1;
    var obj = {};
    obj[name] = value;
    MQTT.publish(
      prefix + 'thing/event/property/post',
      JSON.stringify({
        id: cmdId,
        version: '1.0',
        method: 'thing.event.property.post',
        params: obj
      })
    );
  } else {
    // 先保存,连通之后再上报
    dev_property_update[name] = true;
  }
}

// 主动上报事件
deviceShadow.postEvent = function (name, value) {
  // console.log('call deviceShadow.postEvent');
  dev_event[name] = value;
  dev_event_update[name] = undefined;
  if (dev_mqtt_started == 2) {
    console.log('call MQTT.publish');
    cmdId = cmdId + 1;
    MQTT.publish(
      prefix + 'thing/event/' + name + '/post',
      JSON.stringify({
        id: cmdId,
        version: '1.0',
        method: ('thing.event.' + name + '.post'),
        params: value
      })
    );
  } else {
    // 先保存,连通之后再上报
    dev_event_update[name] = true;
  }
}


//启动服务
deviceShadow.start = function (ntfy_fn) {

  dev_start_nofity_fn = ntfy_fn;

  if (dev_mqtt_started) {
    // 服务已经启动
    dev_start_nofity_err = '服务已经启动';
    process.nextTick(notify_app_start);
    return 0;
  }
  prefix = '/sys/' + dev_config.productKey + '/' + dev_config.deviceName + '/';
  dev_mqtt_started = 1;
  console.log('call  MQTT.start');
  var err = MQTT.start(dev_config, mqtt_start_notify);
  if (err != 0) {
    // 出错
    dev_start_nofity_err = 'error = ' + err;
    process.nextTick(notify_app_start);
  }
  return err;
}


module.exports = deviceShadow;
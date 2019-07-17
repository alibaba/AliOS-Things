//cloudClient  MQTT  Link Developer 

var cloudClient = {};
var connected = false;
var id = 0;
var prefix = null;
var idcbs = {};

// cloudClient 

// config 
// productKey: "xxxxxx",
// deviceName: "xxxxxx",
// deviceSecret: "xxxxxx"

// cb  cloudClient 

cloudClient.start = function(config, cb) {
  if (connected) {
    cb(null, "success");
    return;
  }
  
 //Native MQTT module
  MQTT.start(config, function(error) {
    if (error) {
      connected = false;
      console.log("mqtt Auth fail");
      cb("mqtt Auth fail");
    } else {
      connected = true;
      prefix = "/sys/" + config.productKey + "/" + config.deviceName + "/";
      MQTT.subscribe(prefix + "thing/event/property/post_reply", function(msg, topic) {
        console.log(topic);
      });
      cb(null, "success");
    }
  });
};

//onPropertySet 
// cb 
cloudClient.onPropertySet = function(cb) {
  MQTT.subscribe(prefix + "thing/service/property/set", function(msg, topic) {
    msg && cb && cb(msg);
  });
};

cloudClient.onEvent = function(cb) {};
cloudClient.onService = function(cb) {};

//postProperty 
//cb
cloudClient.postProperty = function(params, cb) {
  if (!connected) {
    cb("MQTT not connected");
    return;
  }

  MQTT.publish(
    prefix + "thing/event/property/post",
    JSON.stringify({
      id: id++,
      version: "1.0",
      method: "thing.event.property.post",
      params: params
    })
  );
};

//postEvent 
// cb 
cloudClient.postEvent = function(eventName, params, cb) {
  if (!connected) {
    cb("MQTT not connected");
    return;
  }

  MQTT.publish(
    prefix + "thing/event/" + eventName + "/post",
    JSON.stringify({
      id: id++,
      version: "1.0",
      method: "thing.event." + eventName + ".post",
      params: params
    })
  );
};

//replySyncService 
cloudClient.replySyncService = function(msgid, data, cb) {
  if (!connected) {
    cb("MQTT not connected");
    return;
  }

  MQTT.publish(
    prefix + "rrpc/response/" + msgid,
    JSON.stringify({
      id: id++,
      code: 200,
      data: data || {}
    })
  );
};

//replyAsyncService 
cloudClient.replyAsyncService = function(serviceName, data, cb) {
  if (!connected) {
    cb("MQTT not connected");
    return;
  }

  MQTT.publish(
    prefix + "thing/service/" + serviceName + "_reply",
    JSON.stringify({
      id: id++,
      code: 200,
      data: data || {}
    })
  );
};

module.exports = cloudClient;

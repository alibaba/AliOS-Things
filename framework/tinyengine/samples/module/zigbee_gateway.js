var config = {
  productKey: "a1iCkQLYR2f",
  deviceName: "mygw",
  deviceSecret: "Q3fm5mTnqKKGIHnvrOZTg5vuWOm4niTo"
};

var addrsCache = {};

var GATEWAY_SYS_PREFIX = "/sys/" + config.productKey + "/" + config.deviceName + "/thing/";
var EVENT_PROPERTY_POST = GATEWAY_SYS_PREFIX + "event/property/post";
var EVENT_PROPERTY_POST_REPLY = GATEWAY_SYS_PREFIX + "event/property/post_reply";
var SERVICE_PROPERTY_SET = GATEWAY_SYS_PREFIX + "service/property/set";
var TOPO_ADD= GATEWAY_SYS_PREFIX + "topo/add";
var TOPO_ADD_REPLY = GATEWAY_SYS_PREFIX + "topo/add_reply";

var EXT_SESSION_PREFIX = "/ext/session/" + config.productKey + "/" + config.deviceName + "/";
var COMBINE_LOGIN = EXT_SESSION_PREFIX + "combine/login";
var COMBINE_LOGIN_REPLY = EXT_SESSION_PREFIX + "combine/login_reply";
  
function alinkJson(addr, method, params) {
  var data = {id: addr, version: "1.0"};
  if (method) {
    data.method = method;
  }
  data.params = params;
  return JSON.stringify(data);
}

function gateWaySubscribe() {
  MQTT.subscribe(SERVICE_PROPERTY_SET, function(msg) { console.log(JSON.stringify(msg)); });
  MQTT.subscribe(EVENT_PROPERTY_POST_REPLY, function(msg) {});
  MQTT.subscribe(TOPO_ADD_REPLY, function(msg) { if (msg.code == 200) subdevLogin(msg.id) });
  MQTT.subscribe(COMBINE_LOGIN_REPLY, function(msg) { if (msg.code == 200) subdevSubscribe(msg.id) });
}

function gateWayAddtopo(addr) {
  if (!addrsCache[addr]) {
      return;
  }
  MQTT.publish(TOPO_ADD, alinkJson(addr, "thing.topo.add", [{deviceName: addrsCache[addr].deviceName, productKey: addrsCache[addr].productKey, sign: addrsCache[addr].sign, signMethod: "hmacsha1", clientId: addrsCache[addr].deviceName}]));
}

function subdevLogin(addr) {
  if (!addrsCache[addr]) {
      return;
  }
  MQTT.publish(COMBINE_LOGIN, alinkJson(addr, null, {deviceName: addrsCache[addr].deviceName, productKey: addrsCache[addr].productKey, sign: addrsCache[addr].sign, signMethod: "hmacsha1", clientId: addrsCache[addr].deviceName, cleanSession: true}));
}

function subdevSubscribe(addr) {
  if (!addrsCache[addr]) {
      return;
  }
 
  MQTT.subscribe("/sys/" + addrsCache[addr].productKey + "/" + addrsCache[addr].deviceName + "/thing/service/property/set", function(msg) { 
    Zigbee.send(addr, JSON.stringify(msg.params), Zigbee.BZB_MESSAGE);
  });
 
  MQTT.subscribe("/sys/" + addrsCache[addr].productKey + "/" + addrsCache[addr].deviceName + "/thing/event/property/post_reply", function(msg) {  
  });
  
  Zigbee.send(addr, addrsCache[addr].deviceName, Zigbee.BZB_CONNECT_REPLY);
}

MQTT.start(config, function(error) {
  if (error) {
    console.log(error);
    return;
  }
  
  console.log("mqtt connected");
  Zigbee.on("message", function(addr, params) {
    // console.log("zigbee on_message");  
    if (addrsCache[addr]) {
      MQTT.publish("/sys/" + addrsCache[addr].productKey + "/" + addrsCache[addr].deviceName + "/thing/event/property/post", alinkJson(addr, "thing.event.property.post", params));  
    }  
  });

  Zigbee.on("connect", function(addr, msg) {
    addrsCache[addr] = {productKey: msg.productKey, deviceName: msg.deviceName, deviceSecret: msg.deviceSecret, sign: msg.sign};
    gateWayAddtopo(addr);
  });

  Zigbee.start({ port: 0x81, service: "dht11|brightness|screen", uartPort: 1, uartBaudrate: 115200 });
  Zigbee.notify();
  
  gateWaySubscribe();

  setInterval(function() {
    MQTT.publish(EVENT_PROPERTY_POST, alinkJson("1", "thing.event.property.post", { Status: 0 }));
  }, 25*1000);
});





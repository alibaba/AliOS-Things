var config={
    productKey: 'a1A2oYj2wef',
    deviceName: 'MK3060P001',
    deviceSecret: 'tibGopWZUeul9JEwjtfcKQG57ItGHa1A'
};

var SYS_PREFIX = '/sys/' + config.productKey + '/' + config.deviceName + '/thing/';
var EVENT_PROPERTY_POST = SYS_PREFIX + 'event/property/post';
var EVENT_PROPERTY_POST_REPLY = SYS_PREFIX + 'event/property/post_reply';
var SERVICE_PROPERTY_SET = SYS_PREFIX + 'service/property/set';
  
function alinkJson(addr, method, params) {
  var data = {id: addr, version: '1.0'};
  if (method) {
    data.method = method;
  }
  data.params = params;
  return JSON.stringify(data);
}

MQTT.start(config, function(error) {
  if (error) {
    console.log(error);
    return;
  }
  
  console.log('mqtt connected');
  MQTT.subscribe(SERVICE_PROPERTY_SET, function(msg) { console.log(JSON.stringify(msg)); });
  MQTT.subscribe(EVENT_PROPERTY_POST_REPLY, function(msg) {});

  setInterval(function() {
    MQTT.publish(EVENT_PROPERTY_POST, alinkJson('1', 'thing.event.property.post', { Status: 0 }));
  }, 5*1000);
});
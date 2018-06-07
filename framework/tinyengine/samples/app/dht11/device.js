
/*
* CurrentTemperature
* https://linkdevelop.aliyun.com/p/a124XKk9O6qua0XK/device/productaccess/#/productaccess/productdetail?productKey=a1cXajyhMQ8&key=definition
* */


// 设备三要素

var config={
    productKey: 'a1cXajyhMQ8',
    deviceName: 'DHT11_1',
    deviceSecret: 'URlaUKkvvb3HbCj6Oz2rBH0cXrvKIzsd'
};

// 该设备所有属性
var properties = [
    "CurrentHumidity",
    "CurrentTemperature"
];

var events = [
    "thing/event/property/post"
];


var services = [
    "thing/service/property/set",
    "thing/service/property/get",
    "thing/event/property/post_reply"
];


function updateThings(method, params) {
    var data = {id: device.eventId, version: '1.0'};
    if (method) {
        data.method = method;
    }
    data.params = params;
    return JSON.stringify(data);
};

function toMethod(event)
{
    var i;
    var j = event.length;
    var ch;
    var ret = '';

    for(i=0; i<j; i++){

        ch = event.charAt(i);

        if( ch == '/' )
            ret = ret + '.';
        else
            ret = ret + ch;
    }
    return ret;
};

var servicesCall = function(msg, topic)
{
    console.log('收到mqtt message. ' + topic);

    process.nextTick(function() {
        if (device.mCallbackFunc) {
            device.mCallbackFunc(topic, msg, device.mUserdata);
        }
    });
};


function Device() {
    this.events = events;
    this.services = services;
    this.properties = properties;
    this.connected = 0;
    this.eventId = 0;
    this.mCallbackFunc = null;
    this.mUserdata = null;
    this.toMethod = toMethod;
};

var device = new Device();

device.setHandler = function(callback, userdate)
{
    device.mCallbackFunc = callback;
    device.mUserdata = userdata;
};

var SYS_PREFIX = '/sys/' + config.productKey + '/' + config.deviceName + '/';

device.update = function(event, obj)
{
    if( device.connected != 2 ){
        return;
    }

    device.eventId ++;

    //console.log(updateThings(toMethod(event), obj));

    MQTT.publish(SYS_PREFIX+event, updateThings(toMethod(event), obj));
};

device.isOnline = function()
{
    console.log('device.connected='+device.connected);

    if( device.connected != 2 )
        return false;

    return true;
};



function subscribe() {

    var id;
    var topic;

    // 订阅所有topic
    for ( id in services )
    {
        console.log(services[id]);

        topic = SYS_PREFIX + services[id];

        console.log('subscribe ' + topic);

        MQTT.subscribe(topic, servicesCall);
    }
};

device.connectCloud = function()
{
    if( device.connected != 0 )
        return ;

    // 正在连接
    device.connected = 1;

    MQTT.start(config, function(error) {

        if (error) {

            console.log(error);

            device.connected = 0;

            // mqtt 网络连接失败, 重试
            console.log('mqtt 网络连接失败, 重试');
            setTimeout(device.connectCloud, 2000);

            return;
        }

        console.log('mqtt connected success');

        subscribe();

        // 连接成功
        device.connected = 2;

        process.nextTick(function(){
            if( device.mCallbackFunc ) {
                device.mCallbackFunc('state', device.connected, device.mUserdata);
            };
        });

    });

};

module.exports.device = device;





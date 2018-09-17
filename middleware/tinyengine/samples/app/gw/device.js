
/*
* 模组网关
* https://linkdevelop.aliyun.com/p/a124XKk9O6qua0XK/gateway/productaccess/#/productaccess/productdetail?productKey=a1P3UMxL1e8&key=development
* */


// 设备三要素

var config={
    productKey: 'a1P3UMxL1e8',
    deviceName: 'GW_1',
    deviceSecret: 'y328sme2kSDCF9o5kwL9McR0fudcFGTV'
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


var addrsCache = {};

function alinkJson(addr, method, params) {
    var data = {id: addr, version: "1.0"};
    if (method) {
        data.method = method;
    }
    data.params = params;
    return JSON.stringify(data);
}

function updateThings(method, params) {
    return alinkJson(gateway.eventId, method, params);
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
        if (gateway.mCallbackFunc) {
            gateway.mCallbackFunc(topic, msg, gateway.mUserdata);
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

var gateway = new Device();

gateway.setHandler = function(callback, userdate)
{
    gateway.mCallbackFunc = callback;
    gateway.mUserdata = userdata;
};

var SYS_PREFIX = '/sys/' + config.productKey + '/' + config.deviceName + '/';

var GATEWAY_SYS_PREFIX = SYS_PREFIX + "thing/";
var EVENT_PROPERTY_POST = GATEWAY_SYS_PREFIX + "event/property/post";
var EVENT_PROPERTY_POST_REPLY = GATEWAY_SYS_PREFIX + "event/property/post_reply";
var SERVICE_PROPERTY_SET = GATEWAY_SYS_PREFIX + "service/property/set";
var TOPO_ADD= GATEWAY_SYS_PREFIX + "topo/add";
var TOPO_ADD_REPLY = GATEWAY_SYS_PREFIX + "topo/add_reply";

var EXT_SESSION_PREFIX = "/ext/session/" + config.productKey + "/" + config.deviceName + "/";
var COMBINE_LOGIN = EXT_SESSION_PREFIX + "combine/login";
var COMBINE_LOGIN_REPLY = EXT_SESSION_PREFIX + "combine/login_reply";


// 网关上报属性
gateway.update = function(event, obj)
{
    if( gateway.connected != 2 ){
        return;
    }

    gateway.eventId ++;

    //console.log(updateThings(toMethod(event), obj));

    MQTT.publish(SYS_PREFIX+event, updateThings(toMethod(event), obj));
};

// 子设备上报属性
gateway.subdevUpdate = function(addr, event, obj)
{
    if( gateway.connected != 2 ){
        return;
    }

    //gateway.eventId ++;
    //console.log(updateThings(toMethod(event), obj));
    //MQTT.publish(SYS_PREFIX+event, updateThings(toMethod(event), obj));

    //MQTT.publish("/sys/" + addrsCache[addr].productKey + "/" + addrsCache[addr].deviceName + "/thing/event/property/post", alinkJson(addr, "thing.event.property.post", obj));

    MQTT.publish("/sys/" + addrsCache[addr].productKey + "/" + addrsCache[addr].deviceName + "/" + event, alinkJson(addr, "thing.event.property.post", obj));

};



gateway.isOnline = function()
{
    console.log('gateway.connected='+gateway.connected);

    if( gateway.connected != 2 )
        return false;

    return true;
};



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
        console.log(addrsCache[addr].deviceName  + ' 收到 set 命令');
        console.log(JSON.stringify(msg));
    });

    MQTT.subscribe("/sys/" + addrsCache[addr].productKey + "/" + addrsCache[addr].deviceName + "/thing/event/property/post_reply", function(msg) {
        console.log(JSON.stringify(msg));
    });

}

function subscribe() {

    MQTT.subscribe(TOPO_ADD_REPLY, function(msg) {
        if (msg.code == 200)
            subdevLogin(msg.id);      // 子设备 login 在线
    });

    MQTT.subscribe(COMBINE_LOGIN_REPLY, function(msg) {
        if (msg.code == 200){

            subdevSubscribe(msg.id);  // 子设备消息订阅

            // 通知App子设备login成功
            process.nextTick(function(){
                if( gateway.mCallbackFunc ) {
                    // 避免重启回调通知
                    if( addrsCache[msg.id].logined != 1){
                        addrsCache[msg.id].logined = 1;
                        gateway.mCallbackFunc('login', msg.id, gateway.mUserdata);
                    }
                };
            });
        }
    });

};

gateway.connectCloud = function()
{
    if( gateway.connected != 0 )
        return ;

    // 正在连接
    gateway.connected = 1;

    MQTT.start(config, function(error) {

        if (error) {

            console.log(error);

            gateway.connected = 0;

            // mqtt 网络连接失败, 重试
            console.log('mqtt 网络连接失败, 重试');
            setTimeout(gateway.connectCloud, 2000);

            return;
        }

        console.log('mqtt connected success');

        subscribe();

        // 连接成功
        gateway.connected = 2;

        process.nextTick(function(){
            if( gateway.mCallbackFunc ) {
                gateway.mCallbackFunc('state', gateway.connected, gateway.mUserdata);
            };
        });

    });

};

// 增加子设备
gateway.subdevAdd = function(addr, params) {

    if (addrsCache[addr]) {
        console.log('该设备已经添加');
        return;
    }

    addrsCache[addr] = params;
    addrsCache[addr].logined = -1;

    MQTT.publish(TOPO_ADD, alinkJson(addr, "thing.topo.add", [{deviceName: params.deviceName, productKey: params.productKey, sign: params.sign, signMethod: "hmacsha1", clientId: params.deviceName}]));
}

module.exports.gateway = gateway;



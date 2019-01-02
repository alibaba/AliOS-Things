
console.log('TinyEngine Demo run');

console.log('========================================\n');
console.log('Welcome to use BoneEngine@lite');
console.log('process.platform=' + process.platform);
console.log('process.version=' + process.version);
console.log('process.uptime()=' + process.uptime());
var version = require('./version.js').appversion;
console.log('app version = ' + version);
console.log('========================================\n');

console.log('产品名称: 光感传感器gy30');

var device = require('./device.js').device;
var gy30 = require('./drivers/gy30/gy30.js').gy30;

function property_get(params)
{

}


var t0 = null;
var gy30_val=[0];

var postEvent = function(val) {
    var obj={};

    var id;
    var attrs = device.properties;
    obj['light'] = val[0];
    var event = device.events[0];
    device.update(event, obj);
};

function nodify()
{

    var lightVal = gy30.get_light();
    console.log('lightVal:'+lightVal);
    if(lightVal == gy30_val[0]){
        return; 
    }
    gy30_val[0] = lightVal;
    // 避免栈溢出, kernel panic,err 1200!
    process.nextTick(function(){
        postEvent(gy30_val);
    });

}


var CloudNotifyCB = function(topic, msg, priv)
{
    console.log('收到topic : ' + topic);

    if( topic == 'state') {
        console.log('设备上云状态: ' + msg);

        // 启动定时器
        if( t0 == null ) {
            console.log('启动定时器...');
            t0 = setInterval(nodify, 10*1000);
        }
    }
    else if( msg ) {
        console.log(msg.method);

        // thing.service.property.get

        if( msg.method ==  device.toMethod(device.services[1]) ){

        }
    }
};


device.setHandler(CloudNotifyCB, null);

var t1 = setInterval(function() {
    var ip = WIFI.getip();
    if(ip != null ){
        console.log('开始连接Ali Iot Cloud');
        console.log('设备IP=' + ip );
        process.nextTick(device.connectCloud);
        clearInterval(t1);
        t1 = null;
    }
}, 1000);

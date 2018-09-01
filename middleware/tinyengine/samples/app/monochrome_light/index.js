console.log('========================================\n');
console.log('Welcome to use BoneEngine@lite');
console.log('process.platform=' + process.platform);
console.log('process.version=' + process.version);
console.log('process.uptime()=' + process.uptime());
var version = require('./version.js').appversion;
console.log('app version = ' + version);
console.log('========================================\n');

console.log('产品名称: 单色LED灯');

var device = require('./device.js').device;
var light = require('./drivers/light.js').light;


// 云端设置属性
function property_set(params)
{
    if(params){

        console.log(params.LightStatus);

        if( light.isOn != params.LightStatus ){
            // 设置light状态
            light.set(params.LightStatus);
            // 云端更新light状态
            // 避免栈溢出, kernel panic,err 1200!
            process.nextTick(function(){
                postEvent(device.properties[0], light.isOn);
            });
        }
    }

}

var CloudNotifyCB = function(topic, msg, priv)
{
    console.log('收到topic : ' + topic);

    if( topic == 'state') {
        console.log('设备上云状态: ' + msg);
        // 云端更新light状态
        // 避免栈溢出, kernel panic,err 1200!
        process.nextTick(function(){
            postEvent(device.properties[0], light.isOn);
        });
    }
    else if( msg ) {
        console.log(msg.method);

        // thing.service.property.set

        if( msg.method ==  device.toMethod(device.services[0]) ){
            //Payload: '{"method":"thing.service.property.set","id":"2159633","params":{"LightStatus":1},"version":"1.0.0"}' (Length: 99)
            property_set(msg.params);
        }
    }
};


device.setHandler(CloudNotifyCB, null);

var t1 = setInterval(function() {
    var ip = getip();
    if(ip != null ){
        console.log('开始连接Ali Iot Cloud');
        console.log('设备IP=' + ip );
        process.nextTick(device.connectCloud);
        clearInterval(t1);
        t1 = null;
    }
}, 1000);


var postEvent = function(name, value) {
    var obj={};

    var id;
    var attrs = device.properties;

    id = attrs.indexOf(name);

    //console.log('name = ' + name + ' index=' + id);

    if( id == null ){
        console.log('该属性不存在');
        return ;
    }

    var attr = attrs[id];
    obj[attr] = value;

    var event = device.events[0];
    device.update(event, obj);
};

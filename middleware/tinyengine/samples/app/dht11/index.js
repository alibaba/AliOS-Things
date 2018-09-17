console.log('========================================\n');
console.log('Welcome to use BoneEngine@lite');
console.log('process.platform=' + process.platform);
console.log('process.version=' + process.version);
console.log('process.uptime()=' + process.uptime());
var version = require('./version.js').appversion;
console.log('app version = ' + version);
console.log('========================================\n');

console.log('产品名称: 温湿度传感器');

var device = require('./device.js').device;
var dht11 = require('./drivers/dht11.js').dht11;


// 云端获取属性
function property_get(params)
{

}


var t0 = null;
var dht11_val=[0,0];

function nodify()
{
    // 云端更新light状态

    var val;

    val = dht11.get();

    if( (val[0] == dht11_val[0] ) && (val[1] == dht11_val[1]) ){
        // 相同值
        console.log('温湿度未发生变化');
        return ;
    }

    dht11_val[0] = val[0];
    dht11_val[1] = val[1];

    console.log('湿度=' + val[0]);
    console.log('温度=' + val[1]);

    // 避免栈溢出, kernel panic,err 1200!
    process.nextTick(function(){
        postEvent(val);
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

        // 避免栈溢出, kernel panic,err 1200!
        process.nextTick(function(){
            postEvent(dht11.get());
        });
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
    var ip = getip();
    if(ip != null ){
        console.log('开始连接Ali Iot Cloud');
        console.log('设备IP=' + ip );
        process.nextTick(device.connectCloud);
        clearInterval(t1);
        t1 = null;
    }
}, 1000);


var postEvent = function(val) {
    var obj={};

    var id;
    var attrs = device.properties;

    obj['CurrentHumidity'] = val[0];
    obj['CurrentTemperature'] = val[1];

    var event = device.events[0];
    device.update(event, obj);
};

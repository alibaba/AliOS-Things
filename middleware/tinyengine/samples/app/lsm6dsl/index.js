console.log('========================================\n');
console.log('Welcome to use BoneEngine@lite');
console.log('process.platform=' + process.platform);
console.log('process.version=' + process.version);
console.log('process.uptime()=' + process.uptime());
var version = require('./version.js').appversion;
console.log('app version = ' + version);
console.log('========================================\n');

console.log('产品名称: 陀螺仪&加速度计传感器LSM6DSL');

var device = require('./device.js').device;
var lsm6dsl = require('./drivers/lsm6dsl.js').lsm6dsl;


function property_get(params)
{

}


var t0 = null;
var lsm6dsl_val=[0,0,0,0,0,0];

var postEvent = function(val) {
    var obj={};

    var id;
    var attrs = device.properties;
    obj['Xacc'] = val[0];
    obj['Yacc'] = val[1];
    obj['Zacc'] = val[2];
    obj['Xgyro'] = val[3];
    obj['Ygyro'] = val[4];
    obj['Zgyro'] = val[5];
    var event = device.events[0];
    device.update(event, obj);
};

function nodify()
{

    var accVal;
    var gyroVal;
    var flag = 0;
    accVal = lsm6dsl.getAcc();
    gyroVal = lsm6dsl.getGyro();

    if((lsm6dsl_val[0]==accVal[0])&&(lsm6dsl_val[1]==accVal[1])&&(lsm6dsl_val[2]==accVal[2])){
        flag += 1;   
    }
    if((lsm6dsl_val[3]==gyroVal[0])&&(lsm6dsl_val[4]==gyroVal[1])&&(lsm6dsl_val[5]==gyroVal[2])){
        flag += 1;   
    }
    if(2==flag)return;
    console.log('xacc:'+accVal[0] + 'yacc:'+accVal[1] + 'zacc:'+accVal[2]);
    console.log('xgyro:'+gyroVal[0] + 'ygyro:'+gyroVal[1] + 'zgyro:'+gyroVal[2]);
    lsm6dsl_val[0] = accVal[0];
    lsm6dsl_val[1] = accVal[1];
    lsm6dsl_val[2] = accVal[2];
    lsm6dsl_val[3] = gyroVal[0];
    lsm6dsl_val[4] = gyroVal[1];
    lsm6dsl_val[5] = gyroVal[2];
    // 避免栈溢出, kernel panic,err 1200!
    process.nextTick(function(){
        postEvent(lsm6dsl_val);
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

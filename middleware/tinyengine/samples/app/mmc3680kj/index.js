console.log('========================================\n');
console.log('Welcome to use BoneEngine@lite');
console.log('process.platform=' + process.platform);
console.log('process.version=' + process.version);
console.log('process.uptime()=' + process.uptime());
var version = require('./version.js').appversion;
console.log('app version = ' + version);
console.log('========================================\n');

console.log('产品名称: 磁力传感器MMC3680KJ');

var device = require('./device.js').device;
var mmc3680kj = require('./drivers/mmc3680kj.js').mmc3680kj;

function property_get(params)
{

}

var t0 = null;
var mmc3680kj_val=[0,0,0];

var postEvent = function(val) {
    var obj={};

    var id;
    var attrs = device.properties;
    obj['Xt'] = val[0];
    obj['Yt'] = val[1];
    obj['Zt'] = val[2];
    var event = device.events[0];
    device.update(event, obj);
};


function nodify()
{
    var val = mmc3680kj.getmemsic();
    if(-1==val[0] && -1==val[1] && -1==val[2]){
        return;   
    }
    console.log('mmc3680kj:x='+val[0]+' y:'+val[1] + ' z:'+val[2]);
    if(val[0] == mmc3680kj_val[0] && val[1] == mmc3680kj_val[1] &&  val[2] == mmc3680kj_val[2]){
        return;
    }
    mmc3680kj_val[0] = val[0];
    mmc3680kj_val[1] = val[1];
    mmc3680kj_val[2] = val[2];
    // 避免栈溢出, kernel panic,err 1200!
    process.nextTick(function(){
        postEvent(mmc3680kj_val);
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
            t0 = setInterval(nodify, 4*1000);
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


console.log('========================================\n');
console.log('Welcome to use BoneEngine@lite');
console.log('process.platform=' + process.platform);
console.log('process.version=' + process.version);
console.log('process.uptime()=' + process.uptime());
var version = require('./version.js').appversion;
console.log('app version = ' + version);
console.log('========================================\n');

console.log('产品名称: 模组网关');

var gateway = require('./device.js').gateway;
var light = require('./drivers/light.js').light;
var dht11 = require('./drivers/dht11.js').dht11;


var CloudNotifyCB = function(topic, msg, priv)
{
    console.log('收到topic : ' + topic);

    if( topic == 'state') {
        console.log('设备上云状态: ' + msg);

        // 避免栈溢出, kernel panic,err 1200!
        process.nextTick(function(){
            addSubDevice();
        });
    }
    else if( topic == 'login') {
        console.log('设备: ' + msg + ' login success');

        if( msg == 'dht11') {
            // 开个定时器获取温湿度
            startDHT11();
        }
    }
    else if( msg ) {

    }
};


gateway.setHandler(CloudNotifyCB, null);

var t1 = setInterval(function() {
    var ip = getip();
    if(ip != null ){
        console.log('开始连接Ali Iot Cloud');
        console.log('设备IP=' + ip );
        process.nextTick(gateway.connectCloud);
        clearInterval(t1);
        t1 = null;
    }
}, 2000);




function addSubDevice()
{

    // 加一个led灯子设备
    var light_config={
        productKey: 'a1Jmy99o5NS',
        deviceName: 'LED_3',
        deviceSecret: 'XyoqgUUeUdjBWINJjuxGtLEwBwUXC4wu'
    };

    var sign = MQTT.sign(light_config);
    light_config['sign'] = sign;

    //console.log('sign = ' + light_config.sign);

    gateway.subdevAdd('light', light_config);


    // 加一个dht11子设备
    var dht11_config={
        productKey: 'a1cXajyhMQ8',
        deviceName: 'DHT11_3',
        deviceSecret: '172omxh3Tokir1AcmEcDuj3VJcSpP73h'
    };

    sign = MQTT.sign(dht11_config);
    dht11_config['sign'] = sign;

    gateway.subdevAdd('dht11', dht11_config);

}

function updateLight(on)
{
    var event = gateway.events[0];

    var obj={};
    obj['LightStatus'] = on;

    gateway.subdevUpdate('light', event, obj);

    light.set(on);
}

var dht11_val=[0,0];
function updateDHT11()
{
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

    var obj={};
    obj['CurrentHumidity'] = val[0];
    obj['CurrentTemperature'] = val[1];

    // 避免栈溢出, kernel panic,err 1200!
    process.nextTick(function(){
        gateway.subdevUpdate('dht11', gateway.events[0], obj);
    });
}


var t2 = null;

function startDHT11()
{
    if( t2 )
        return ;

    t2 = setInterval(function() {

        updateDHT11();

        if( dht11_val[0] < 50 )
            updateLight(0);
        else
            updateLight(1);

    }, 10000);
}
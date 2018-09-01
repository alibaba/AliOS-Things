console.log('========================================\n');
console.log('Welcome to use BoneEngine@lite');
console.log('process.platform=' + process.platform);
console.log('process.version=' + process.version);
console.log('process.uptime()=' + process.uptime());
var version = require('./version.js').appversion;
console.log('app version = ' + version);
console.log('========================================\n');

console.log('产品名称: 温湿度传感器SHTC1');

var device = require('./device.js').device;
var shtc1 = require('./drivers/shtc1.js').shtc1;

console.log('lcd test for developkit\n');


function rgb565(r, g, b) {
    var rvalue = r >> 3;
    rvalue = rvalue << 11;
    var gvalue = g >> 2;
    gvalue = gvalue << 5;
    var bvalue = b >> 3;
    return (rvalue + gvalue + bvalue);

}

var lcd_config = {};
lcd_config.height = 240;
lcd_config.width = 240;
var ret = LCD.open(lcd_config);
console.log('LCD.open try!\n');
if (0 != ret) {
    console.log('LCD.open fail!\n');
    return;
}
 /*
     LCD.fill的两个作用:
     1.画填充/不填充矩形
     2.写某区域前，擦除该区域原来数据
     */
 var lcd_fill = {};
 lcd_fill.color = rgb565(0xff, 0xff, 0xff);
 lcd_fill.sx = 0;
 lcd_fill.sy = 0;
 lcd_fill.ex = 239;
 lcd_fill.ey = 239;
 lcd_fill.fill = 1;
 LCD.fill(lcd_fill);
 console.log('LCD.open clean!\n');

 var lcd_show = {};
 lcd_show.color = rgb565(255, 0, 0);
 lcd_show.x = 50;
 lcd_show.y = 20;
 lcd_show.newline = 1;
 lcd_show.str = 'Bone Engine';
 lcd_show.size = 24;
 LCD.show(lcd_show);
 console.log('LCD.open display!\n');



function property_get(params)
{

}

function lcd_initlize_content()
{
    /*
    LCD.fill的两个作用:
    1.画填充/不填充矩形
    2.写某区域前，擦除该区域原来数据
    */
    var lcd_fill = {};
    lcd_fill.color = rgb565(0xff, 0xff, 0xff);
    lcd_fill.sx = 0;
    lcd_fill.sy = 0;
    lcd_fill.ex = 239;
    lcd_fill.ey = 239;
    lcd_fill.fill = 1;
    LCD.fill(lcd_fill);
    console.log('LCD.open clean!\n');

    var lcd_show = {};
    lcd_show.color = rgb565(255, 0, 0);
    lcd_show.x = 50;
    lcd_show.y = 20;
    lcd_show.newline = 1;
    lcd_show.str = 'Bone Engine';
    lcd_show.size = 36;
    LCD.show(lcd_show);
    console.log('LCD.open display!\n');
}

function lcd_show_shtc1(temp,humi)
{
    var lcd_fill = {};
    var lcd_show = {};

    lcd_fill.color = rgb565(0xff, 0xff, 0xff);
    lcd_fill.sx = 0;
    lcd_fill.sy = 120;
    lcd_fill.ex = 240;
    lcd_fill.ey = 160+36;
    lcd_fill.fill = 1;
    LCD.fill(lcd_fill);
    
    lcd_show.x = 20;
    lcd_show.y = 120;
    lcd_show.newline = 0;
    lcd_show.size = 24;
    if (temp == 0) {
        lcd_show.str = "tempture: " + "  C";
    } else {
        lcd_show.str = "tempture: " + temp + "  C";
    }
    
    lcd_show.color = rgb565(0, 0, 0xee);
    LCD.show(lcd_show);

    lcd_show.y= 160;

    if (humi == 0){
        lcd_show.str = "humidity: " + "  RH%";
    }else{
        lcd_show.str = "humidity: " + humi + "  RH%";
    }
    LCD.show(lcd_show);
}

var t0 = null;
var shtc1_val=[0,0];

var postEvent = function (val) {
    var obj = {};

    var id;
    var attrs = device.properties;
    obj['temp'] = val[0];
    obj['humi'] = val[1];
    var event = device.events[0];
    lcd_show_shtc1(val[0], val[1]);
    device.update(event, obj);
};

function nodify() {

    var val = shtc1.getTempHumi();
    console.log('shtc1:temp=' + val[0] + ' humi:' + val[1]);
    if (val[0] == shtc1_val[0] && val[1] == shtc1_val[1]) {
        return;
    }
    shtc1_val[0] = val[0];
    shtc1_val[1] = val[1];
    // 避免栈溢出, kernel panic,err 1200!
    process.nextTick(function () {
        postEvent(shtc1_val);
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
            t0 = setInterval(nodify, 3*1000);
        }
    }
    else if( msg ) {
        console.log(msg.method);
        //调试下发指令例子：{"dynamic_disyplay":{"x_axis":20,"y_axis":60,"content":"this is boneengine","newline":0,"size":24}}
        // thing.service.property.get
        if( msg.method ==  device.toMethod(device.services[0]) ){
                console.log(msg.params.title_display);
                console.log(msg.params.dynamic_disyplay.content);
                console.log(msg.params.dynamic_disyplay.x_axis);
                console.log(msg.params.dynamic_disyplay.y_axis);
                console.log(msg.params.dynamic_disyplay.newline);
                lcd_show.color = rgb565(0xd0, 0x20, 0x90);
                lcd_show.x = msg.params.dynamic_disyplay.x_axis;
                lcd_show.y = msg.params.dynamic_disyplay.y_axis;
                lcd_show.newline = msg.params.dynamic_disyplay.newline;
                lcd_show.str = msg.params.dynamic_disyplay.content;
                lcd_show.size = msg.params.dynamic_disyplay.size;
                var lcd_fill = {};
                lcd_fill.color = rgb565(0xff, 0xff, 0xff);
                lcd_fill.sx = 0;
                lcd_fill.sy = lcd_show.y;
                lcd_fill.ex = 240;
                lcd_fill.ey = lcd_show.y + lcd_show.size;
                lcd_fill.fill = 1;
                LCD.fill(lcd_fill);
                LCD.show(lcd_show);
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

lcd_show_shtc1(0, 0);


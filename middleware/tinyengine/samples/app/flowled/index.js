console.log('========================================\n');
console.log('Welcome to use BoneEngine@lite');
console.log('process.platform=' + process.platform);
console.log('process.version=' + process.version);
console.log('process.uptime()=' + process.uptime());
var version = require('./version.js').appversion;
console.log('app version = ' + version);
console.log('========================================\n');

console.log('产品名称: DevelopKit@waterLed');
var waterlight = require('./drivers/waterlights.js').waterlight;
var  led_handle = [5,1,2,4,3];

var  on_led = 0;
var  is_left = 0;

var i = 0;
for(i=0;i<5;i++){
    waterlight.set(led_handle[i],1);   
}

var blink_timeout = 200;
var t = setInterval(function() {
    var i = 0;
    for(i=0;i<5;i++){
        if(i == on_led){
            waterlight.set(led_handle[i],0);
        }else{
            waterlight.set(led_handle[i],1);   
        }
    }
    if(0 == is_left){
        on_led = on_led + 1;
        on_led = on_led % 5;
        if(4 == on_led){
            is_left = 1;   
        }
    }else{
        on_led = on_led - 1;
        if(0 == on_led){
            is_left = 0;   
        }   
    }


}, blink_timeout);

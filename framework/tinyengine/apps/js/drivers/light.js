

// GPIO 输出模式时,默认为0 高阻，LED灯亮
// 外接3.3 VDC来驱动LED二级管

function Light() {
    this.isOn = 1;
    this.handle = GPIO.open('monochrome_light');
};

var light = new Light();


light.set = function(on){
    var val = 1;
    if( on )
        val = 0;

    this.isOn = on;

    GPIO.write(this.handle, val);

    print('set light ' + on);
};

module.exports.light = light;
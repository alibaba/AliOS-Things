var gpio = require('gpio');
var spi = require('spi');
var sh1106 = require('./sh1106.js');


console.log('Hello')

var oled_dc = gpio.open({
    id: 'oled_dc',
    success: function () {
        console.log('gpio: open oled_dc success')
    },
    fail: function () {
        console.log('gpio: open oled_dc failed')
    }
});

var oled_res = gpio.open({
    id: 'oled_res',
    success: function () {
        console.log('gpio: open oled_res success')
    },
    fail: function () {
        console.log('gpio: open oled_res failed')
    }
});

var oled_spi = spi.open({
    id: 'oled_spi',
    success: function () {
        console.log('gpio: open oled_spi success')
    },

    fail: function () {
        console.log('gpio: open oled_spi failed')
    }
});

sh1106.init(132, 64, oled_spi, oled_dc, oled_res, undefined);
sh1106.fill(1);
sh1106.show();

while (1) {
    sh1106.fill(1)
    sh1106.show()

    sh1106.fill(0)
    sh1106.show()
}
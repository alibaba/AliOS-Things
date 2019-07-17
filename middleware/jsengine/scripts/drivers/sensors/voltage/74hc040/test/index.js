var I74HC = require('74hc040/src');
var handle = new I74HC('i74hc.A1');
var index = 0;
setInterval(function() {
    index = 1 - index;
    handle.SetVal(index);
}, 2000);
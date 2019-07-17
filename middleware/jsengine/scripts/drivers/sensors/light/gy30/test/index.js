var gy30 = require('gy30/src/index.js');
var handle = new gy30('gy30');
var t1 = setInterval(function() {
    var lightVal = handle.get_light();
    console.log('lightVal:'+lightVal);
}, 2000);


 


var temt6000 = require('temt6000/src/index.js');

var handle = new temt6000('temt6000');
var t = setInterval(function () {
  var val = handle.getVal();
  console.log('adc val:' + val);
}, 1000);

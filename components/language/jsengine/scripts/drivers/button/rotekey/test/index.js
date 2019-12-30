var ROTEKEY = require('rotekey/src');
var handle = new ROTEKEY('rotekey.sw', 'rotekey.dt', 'rotekey.clk');

handle.OnPress(function (val) {
  console.log('perss it!');
});


handle.OnDir(function (val) {
  console.log('val:' + val);
});

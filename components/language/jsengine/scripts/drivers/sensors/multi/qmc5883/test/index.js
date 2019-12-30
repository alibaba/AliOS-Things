var qmc5883 = require('qmc5883/src/index.js');
var handle = new qmc5883('qmc5883');
setInterval(function () {
  var val = handle.getXYZ();
  console.log('qmc5883->x:' + val[0] + 'y:' + val[1] + ' z:' + val[2]);
}, 1000);
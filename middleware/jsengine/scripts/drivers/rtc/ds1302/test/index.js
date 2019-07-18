var ds1302 = require('ds1302/src/index.js');
var handle = new ds1302('ds1302');

setInterval(function () {
  var val = handle.getTime();
  console.log('ds1302->' + val[2] + ':' + val[1] + ':' + val[0]);
}, 1000);
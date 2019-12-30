var ATGM336H = require('atgm336h/src');
var handle = new ATGM336H('atgm336h');

handle.recv(null, null, function (data) {
  console.log('gpsInfo:' + data);
});
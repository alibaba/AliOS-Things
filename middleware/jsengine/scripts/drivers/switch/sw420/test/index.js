
var sw420 = require('sw420/src/index.js');
var handle = new sw420('sw420.do');
setInterval(function () {
  var val = handle.getVal();
  console.log('sw420:' + val);
}, 100);
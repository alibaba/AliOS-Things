
var lm393 = require('lm393/src/index.js');
var handle = new lm393('lm393');
setInterval(function () {
  var val = handle.check();
  console.log('val:' + val);
}, 2000);
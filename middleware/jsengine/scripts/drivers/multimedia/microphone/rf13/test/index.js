var rf13 = require('rf13/src/index.js');
var handle = new rf13('rf13');
var t1 = setInterval(function () {
  var val = handle.get_sound();
  console.log('sound:' + val);
}, 1000);
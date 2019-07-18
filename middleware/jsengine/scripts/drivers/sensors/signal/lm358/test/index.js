
var lm358 = require('lm358/src/index.js');
var val = 0;
var handle = new lm358('lm358');
var t = setInterval(function () {
  handle.setVal(val);
  val = val + 1;
  if (255 == val) {
    val = 0;
  }
}, 200);

var czn15e = require('czn15e/src/index.js');
var handle = new czn15e('czn15e');
var t1 = setInterval(function () {
  var val;
  val = handle.get();
  if (1 == val) {
    console.log("sound:NO");
  } else {
    console.log("sound:YES");
  }
}, 1000);
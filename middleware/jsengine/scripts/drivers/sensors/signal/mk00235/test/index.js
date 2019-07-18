var MK00235 = require('mk00235/src');
var handle = new MK00235('mk00235');

handle.OnRain(function (val) {
  console.log("get rain!");
});

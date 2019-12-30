var st7735 = require('st7735/src/index.js');
var handle = new st7735();
handle.fill(0, 0, 128, 128, handle.rgb565(0, 0, 0), 1);
var index = 0;
var numCount = 0;
setInterval(function () {
  numCount = numCount + 1;
  numCount = numCount % 100;
  handle.fill(20, 20, 100, 100, handle.rgb565(0, 0, 0), 1);
  handle.show(50, 40, '' + numCount, 1, handle.rgb565(0x00, 0, 0xee), 36);
}, 1000);

var tube595 = require('tube595/src/index.js');
var handle = new tube595('tube595.sclk', 'tube595.rclk', 'tube595.io');
var index = 1;
var blink = 1000;
var t = setInterval(function () {
  handle.set(index);
  index += 1;
  if (5 == index) {
    index = 1;
  }
}, blink);
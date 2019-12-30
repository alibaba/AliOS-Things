var isd1820 = require('isd1820/src/index.js');
var handle = new isd1820('isd1820.play', 'isd1820.record')
var index = 0;
setInterval(function () {
  if (0 == index) {
    console.log('start record!');
    handle.startRecord();
  } else if (1 == index) {
    console.log('start play!');
    handle.stopRecord();
    handle.startPlay();
  }
  index = index + 1;
  if (2 == index) {
    index = 0;
  }
}, 15000);

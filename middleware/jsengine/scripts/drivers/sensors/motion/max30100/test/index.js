var MAX30100 = require('max30100/src');
var handle = new MAX30100('max30100');

setInterval(function () {
  var data = handle.readData();
  if (data[0] < 0 && data[1] < 0) {
    return;
  }
  console.log("Blood Oxygen:" + data[0] + " BeatHeat:" + data[1]);
}, 2000);
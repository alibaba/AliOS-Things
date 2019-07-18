var XD3144 = require('xd3144/src');
var handle = new XD3144('xd3144');
handle.getSpeed(function (val) {
  console.log("Speed:" + val);
});
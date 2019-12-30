var encoder = require("encoder/src/index.js");
var left_encoder = new encoder('encoder.left0', 'encoder.left1');
var right_encoder = new encoder('encoder.right0', 'encoder.right1');

left_encoder.config(500);
right_encoder.config(500);

var leftEncoderCb = function (val) {
  console.log('leftEncoderCb:' + val);
};

var rightEncoderCb = function (val) {
  console.log('rightEncoderCb:' + val);
};

left_encoder.start(leftEncoderCb);
right_encoder.start(rightEncoderCb); 

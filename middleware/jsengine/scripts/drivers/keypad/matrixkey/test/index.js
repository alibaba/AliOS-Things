var matrixkeypad = require('matrixkey/src/index.js');
var handle = new matrixkeypad('matrixkeypad.C1', 'matrixkeypad.C2', 'matrixkeypad.C3', 'matrixkeypad.C4', 'matrixkeypad.R1', 'matrixkeypad.R2', 'matrixkeypad.R3', 'matrixkeypad.R4');
var keycb = function (cval, rval) {
  console.log('cval: ' + cval + 'rval: ' + rval);
};
handle.init(keycb);
setInterval(function () {
  handle.scan();
}, 100);


var mpu9250 = require('mpu9250/src/index.js');
var handle = new mpu9250('mpu9250');

var getVal = function () {
  var acc = handle.getAcc();
  console.log('acc.x:' + acc[0] + ' acc.y:' + acc[1] + ' acc.z:' + acc[2]);
  var gyro = handle.getGyro();
  console.log('gyro.x:' + gyro[0] + ' gyro.y:' + gyro[1] + ' gyro.z:' + gyro[2]);
}

var t = setInterval(getVal, 2000);
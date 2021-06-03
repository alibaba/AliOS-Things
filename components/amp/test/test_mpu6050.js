/*
Please add this section into app.json when run this script as app.js.
This configuration is designed for haas100 edu k1.
"spl06": {
    "type": "I2C",
    "port": 1,
    "addrWidth": 7,
    "freq": 400000,
    "mode": "master",
    "devAddr": 105
}
*/

console.log('testing mpu6050...');

var mpu6050 = require('./mpu6050.js');

mpu6050.init("mpu6050");

var temp = mpu6050.get_Temperature();
console.log("mpu6050 temp is: " , temp);

var gy = mpu6050.get_Gyroscope();
console.log("mpu6050 gyro is: " , gy[0], gy[1], gy[2]);
var ac = mpu6050.get_Accelerometer();
console.log("mpu6050 acc is: " , ac[0], ac[1], ac[2]);

mpu6050.deinit();
console.log("test mpu6050 success!");

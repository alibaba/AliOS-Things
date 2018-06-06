
var lsm6dsl_Handle = I2C.open('lsm6dsl');
setInterval(function(){
	var acc = HW.getLsm6dslAcc(lsm6dsl_Handle);
	console.log('gx='+acc[0] + ' gy='+acc[1] + ' gz='+acc[2]);
	var gyro = HW.getLsm6dslGyro(lsm6dsl_Handle);
	console.log('x='+gyro[0] + ' y='+gyro[1] + ' z='+gyro[2]);
},3000);

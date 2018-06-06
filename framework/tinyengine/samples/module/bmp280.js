
var bmp280_Handle = I2C.open('bmp280');;
setInterval(function(){
	var pressValue = HW.getBmp280(bmp280_Handle);
	console.log('pressureValue:'+pressValue+'hPa');
},3000);


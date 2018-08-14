var ltr553_Handle = I2C.open('ltr553');
setInterval(function(){
	HW.getltr553(ltr553_Handle,function(als,ps){
		console.log('ltr553:als='+als+' ps='+ps);
	});
},3000);



var mmc3680kj_Handle = I2C.open('mmc3680kj');
setInterval(function(){
	HW.getMmc3680kj(mmc3680kj_Handle,function(x,y,z){
		console.log('mmc3680kj:x='+x+' y='+y + ' z='+z);
	});
},3000);

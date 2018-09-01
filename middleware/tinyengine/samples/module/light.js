var light_handle = ADC.open('light');
setInterval(function(){
	var i = 0;
	var LigthValue = 0;
	for(i=0;i<10;i++){
		LigthValue += ADC.read(light_handle);
	}
	LigthValue = LigthValue/10;
	if(LigthValue > 4500){
		LigthValue = 4500;
	}
	LigthValue = 100 - LigthValue/45;
	console.log('LigthValue:'+LigthValue);
},5000);



var led_handle = DAC.open('led');
var cur_voltage = DAC.getVol(led_handle);
console.log('cur_voltage:'+cur_voltage);
setInterval(function(){

	cur_voltage  = cur_voltage + 1;
	if(cur_voltage >= 255){
		cur_voltage = 0;
	}
	DAC.setVol(led_handle,cur_voltage);
},50);


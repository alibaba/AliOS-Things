print('buzzer uart test.....................');
var buzzer_handle=PWM.start('buzzer'); 
var cur_duty = PWM.getDuty(buzzer_handle);
console.log('cur_duty:'+cur_duty);
setInterval(function() {
	cur_duty += 1;
	if(cur_duty >= 100)cur_duty=0;
	PWM.setDuty(buzzer_handle,cur_duty);
}, 500);
print('end buzzer test........................');


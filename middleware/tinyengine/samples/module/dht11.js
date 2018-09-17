

var dht11_handle = GPIO.open('dht11');
setInterval(function() {
		var dht11_buff = HW.getDht11(dht11_handle);
		console.log('dht11->hum:'+dht11_buff[0]+ '  temp:'+dht11_buff[1]);
}, 5000);


import * as gpio from 'gpio';

var led = gpio.open({
  id: 'led',
  success: function() {
      console.log('gpio: open led success')
  },
  fail: function() {
      console.log('gpio: open led failed')
  }
});

var vol = 0;
setInterval(function() {
    vol = 1 - vol;
	led.writeValue(vol);
	console.log('gpio: led set value ' + vol);
}, 2000);



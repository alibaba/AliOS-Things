var gpio = require('gpio');

var led = gpio.open({
  id: 'led',
  success: function() {
      console.log('gpio: open led success')
  },
  fail: function() {
      console.log('gpio: open led failed')
  }
});

var key = gpio.open({
  id: 'key',
  success: function() {
      console.log('gpio: open key success')
  },
  fail: function() {
      console.log('gpio: open key failed')
  }
});

var vol = 0;
var irqCnt = 0;
key.onIRQ({
  cb: function() {
	irqCnt = irqCnt + 1;
	console.log('gpio: irq count ' + irqCnt); 
	vol = 1 - vol;
	led.writeValue(vol);
	console.log('gpio: led set value ' + vol);
	
	vol = led.readValue();
	console.log('gpio: led get value ' + vol);
  }
});

console.log('gpio: ready to test')

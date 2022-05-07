var gpio = require('gpio');

var output = gpio.open({
  id: 'gpio_output',
  success: function() {
    console.log('gpio: open output success')
  },
  fail: function() {
    console.log('gpio: open output failed')
  }
});

var value = 1;
setInterval(function() {
  value = 1 - value;
  output.writeValue(value);
  console.log('gpio: gpio write value ' + output.readValue());
}, 1000);

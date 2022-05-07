import * as gpio from 'gpio';

var inputobj = gpio.open({
  id: 'gpio_input',
  success: function success() {
    console.log('gpio: open led success');
  },
  fail: function fail() {
    console.log('gpio: open led failed');
  }
});

setInterval(function() {
  var value = inputobj.readValue();
  console.log('gpio: read gpio value ' + value);
}, 1000);

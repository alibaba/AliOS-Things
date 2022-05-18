var gpio = require('gpio');

/* open GPIO KEY1 */
var KEY1 = gpio.open({
  id: 'KEY1',
  success: function() {
    console.log('gpio: open KEY1 success')
  },
  fail: function() {
    console.log('gpio: open KEY1 failed')
  }
});

/* read KEY1 input every 1 second */
var value;
setInterval(function() {
  value = KEY1.readValue();
  console.log('gpio: read KEY1 value ' + value);
}, 1000);
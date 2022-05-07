var gpio = require('gpio');

/* open GPIO KEY1 */
var KEY2 = gpio.open({
  id: 'KEY2',
  success: function() {
    console.log('gpio: open KEY2 success')
  },
  fail: function() {
    console.log('gpio: open KEY2 failed')
  }
});

KEY2.onIRQ({
  cb: function() {
  console.log('gpio: KEY2 interrupt happens');
  }
});
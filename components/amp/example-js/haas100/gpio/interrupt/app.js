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

KEY1.onIRQ({
  cb: function() {
  console.log('gpio: KEY1 interrupt happens');
  }
});
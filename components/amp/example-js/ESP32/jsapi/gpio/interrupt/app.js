var gpio = require('gpio');

var irq = gpio.open({
  id: 'gpio_irq',
  success: function() {
    console.log('gpio: open irq success')
  },
  fail: function() {
    console.log('gpio: open irq failed')
  }
});

irq.onIRQ({
  cb: function() {
  console.log('gpio: irq interrupt happens');
  }
});

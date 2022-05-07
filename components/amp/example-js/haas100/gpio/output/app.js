var gpio = require('gpio');

/* open GPIO LED1 */
var LED1 = gpio.open({
  id: 'L1',
  success: function() {
    console.log('gpio: open LED1 success')
  },
  fail: function() {
    console.log('gpio: open LED1 failed')
  }
});

/* open GPIO LED1 */
var LED2 = gpio.open({
  id: 'L2',
  success: function() {
    console.log('gpio: open LED1 success')
  },
  fail: function() {
    console.log('gpio: open LED1 failed')
  }
});

/* open GPIO LED1 */
var LED3 = gpio.open({
  id: 'L3',
  success: function() {
    console.log('gpio: open LED1 success')
  },
  fail: function() {
    console.log('gpio: open LED1 failed')
  }
});

/* open GPIO LED1 */
var LED4 = gpio.open({
  id: 'L4',
  success: function() {
    console.log('gpio: open LED1 success')
  },
  fail: function() {
    console.log('gpio: open LED1 failed')
  }
});

/* open GPIO LED1 */
var LED5 = gpio.open({
  id: 'L5',
  success: function() {
    console.log('gpio: open LED1 success')
  },
  fail: function() {
    console.log('gpio: open LED1 failed')
  }
});

/* toggle LED1 every 1 second */
var value = 1;
setInterval(function() {
  value = 1 - value;
  LED1.writeValue(value);
  LED2.writeValue(value);
  LED3.writeValue(value);
  LED4.writeValue(value);
  LED5.writeValue(value);
  console.log('gpio: LED1 write value ' + value);
}, 1000);
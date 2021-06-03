var adc = require('adc');

var vol = adc.open({
  id: 'adc1',
  success: function() {
    console.log('adc: open adc success')
  },
  fail: function() {
    console.log('adc: open adc failed')
  }
});

setInterval(function() {
    console.log('adc: adc value ' + vol.readValue())
}, 2000);


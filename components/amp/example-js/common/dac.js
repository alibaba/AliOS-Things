var dac = require('dac');

// led灯
var vol = dac.open({
  id: 'light',
  success: function() {
    console.log('open dac success')
  },
  fail: function() {
    console.log('open dac failed')
  }
});

vol.writeValue(65536 / 2)

var value = vol.readValue();

console.log('voltage value is ' + value)

vol.close();
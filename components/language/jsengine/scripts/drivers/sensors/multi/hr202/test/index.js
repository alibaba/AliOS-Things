var HR202 = require('hr202/src');
var handle = new HR202('hr202');

handle.OnHum(function (flag) {
  console.log('Hum!');
});
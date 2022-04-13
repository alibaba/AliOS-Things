var spi = require('spi');

var msgbuf = [0x10, 0xee]
// led灯
var sensor = spi.open({
  id: 'spi',
  success: function() {
    console.log('open spi success')
  },
  fail: function() {
    console.log('open spi failed')
  }
});

sensor.write(msgbuf)
var value = sensor.read(2)

console.log('sensor value is ' + value)


sensor.close();
var i2c = require('i2c');

var memaddr = 0x18
var msgbuf = [0x10, 0xee]
// led灯
var sensor = i2c.open({
  id: 'sensor',
  success: function() {
    console.log('open i2c success')
  },
  fail: function() {
    console.log('open i2c failed')
  }
});

sensor.write(msgbuf)
var value = sensor.read(2)

console.log('sensor value is ' + value)

sensor.writeMem(memaddr, msgbuf)


var vol = sensor.readMem(memaddr, 2)
console.log('sensor read mem vol is ' + vol)


sensor.close();
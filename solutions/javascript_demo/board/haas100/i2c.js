import * as i2c from 'i2c'

var memaddr = 0x18

var msgbuf = [0x10, 0xee]

var sensor = i2c.open({
    id: 'I2C0',
    success: function () {
        console.log('open i2c success')
    },
    fail: function () {
        console.log('open i2c failed')
    }
});

sensor.write(msgbuf)
var value = sensor.read(2)

console.log('sensor read ' + '0x' + value[0].toString(16) + ' 0x' + value[1].toString(16))

sensor.writeMem(memaddr, msgbuf)

var vol = sensor.readMem(memaddr, 2)
console.log('sensor read mem vol is ' + '0x' + value[0].toString(16) + ' 0x' + value[1].toString(16))

sensor.close();
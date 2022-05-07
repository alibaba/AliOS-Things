import * as adc from 'adc'

var vbattery = adc.open({
    id: 'ADC0',
    success: function () {
        console.log('adc: open adc success')
    },
    fail: function () {
        console.log('adc: open adc failed')
    }
});

var value = vbattery.readValue()

console.log('adc value is ' + value)
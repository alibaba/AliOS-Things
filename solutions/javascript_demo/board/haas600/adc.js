import * as adc from 'adc'

var vol = adc.open({
    id: 'battery',
    success: function () {
        console.log('adc: open adc success')
    },
    fail: function () {
        console.log('adc: open adc failed')
    }
});

var value = vol.readValue()

console.log('adc value is ' + value)





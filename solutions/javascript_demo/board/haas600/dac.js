import * as dac from 'dac'

// led灯
var vol = dac.open({
    id: 'voltage',
    success: function () {
        console.log('open dac success')
    },
    fail: function () {
        console.log('open dac failed')
    }
});

vol.writeValue(65536 / 2)

var value = vol.readValue();

console.log('voltage value is ' + value)

vol.close();



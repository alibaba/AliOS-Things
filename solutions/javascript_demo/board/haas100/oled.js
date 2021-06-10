// {
//     "version": "1.0.0",
//     "io": {
//         "oled_dc": {
//             "type": "GPIO",
//             "port": 28,
//             "dir": "output",
//             "pull": "pulldown"
//         },
//         "oled_res": {
//             "type": "GPIO",
//             "port": 30,
//             "dir": "output",
//             "pull": "pulldown"
//         },
//         "oled_spi": {
//             "type": "SPI",
//             "port": 1,
//             "mode": "mode3",
//             "freq": 26000000
//         }
//     },
//     "debugLevel": "DEBUG"
// }

import * as spi from 'spi'
import * as gpio from 'gpio'
import SH1106 from './sh1106.js'

var oled_dc = gpio.open({
    id: "oled_dc",
    success: function () {
        console.log('gpio: open oled_dc success')
    },
    fail: function () {
        console.log('gpio: open oled_dc failed')
    }
});

var oled_res = gpio.open({
    id: "oled_res",
    success: function () {
        console.log('gpio: open oled_res success')
    },
    fail: function () {
        console.log('gpio: open oled_res failed')
    }
});

var oled_spi = spi.open({
    id: "oled_spi",
    success: function () {
        console.log('gpio: open oled_spi success')
    },

    fail: function () {
        console.log('gpio: open oled_spi failed')
    }
});

console.log("look here!!!!")

let dispaly = new SH1106(132, 64, oled_spi, oled_dc, oled_res, undefined)
dispaly.open()

while (1) {
    dispaly.fill(1)
    dispaly.show()

    dispaly.fill(0)
    dispaly.show()
}

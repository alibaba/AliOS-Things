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

var oled_dc = gpio.open({ id: "oled_dc" });
var oled_res = gpio.open({ id: "oled_res" });
var oled_spi = spi.open({ id: "oled_spi" });

var dispaly = new SH1106(132, 64, oled_spi, oled_dc, oled_res, undefined)
dispaly.open()

var HAAS_XBM_width = 23
var HAAS_XBM_height = 9

var HAAS_XBM = [0xff, 0xff, 0xfe, 0x80, 0x00, 0x02, 0xa4, 0xc6, 0x7a, 0xa5, 0x29, 0x42, 0xbd, 0xef, 0x7a, 0xa5, 0x29, 0x0a, 0xa5, 0x29, 0x7a, 0x80, 0x00, 0x02, 0xff, 0xff, 0xfe]
dispaly.draw_XBM(40, 20, HAAS_XBM_width, HAAS_XBM_height, HAAS_XBM)
dispaly.show()

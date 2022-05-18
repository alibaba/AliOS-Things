// app.js
import * as i2c from 'i2c';

// 初始化对应的I2C
var sensor = i2c.open({
  id: 'sensor',
  success: function () {
    // 初始化成功
    console.log('open i2c success')
  },
  fail: function () {
    // 初始化失败
    console.log('open i2c failed')
  }
});

setInterval(function () {
  // 切换成温度模式
  sensor.write([0xF3]);
  sleepMs(30)
  // 代码功能：原始数据格式为[142,124]转成0x8e7c，再解析成数值36476
  var tempData = Number('0x' + sensor.read(2).map((i) => ((i < 10 ?  '0': '') + i.toString(16))).toString().replace(/,/g, ''));
  // 温度公式，可参考Si7006的datasheet
  var temp = ((175.72 * tempData) / 65536 - 46.85).toFixed(2);
  console.log('temp data is ' + temp);

  sleepMs(30);

  // 切换成湿度模式
  sensor.write([0xF5]);
  sleepMs(30);
  // 代码功能：原始数据格式为[142,124]转成0x8e7c，再解析成数值36476
  var humiData = Number('0x' + sensor.read(2).map((i) => ((i < 10 ?  '0': '') + i.toString(16))).toString().replace(/,/g, ''));
  // 湿度公式，可参考Si7006的datasheet
  var humi = ((125 * humiData) / 65536 - 6).toFixed(2);
  console.log('humi data is ' + humi);
}, 1000);

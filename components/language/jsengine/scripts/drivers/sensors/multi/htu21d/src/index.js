/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var delay = function (val) {
  var i = 0;
  var tmp = 0;
  for (i = 0; i < val; i++) {
    tmp = 0;
  }
};

var htu21d = function (id) {
  this.handle = I2C.open(id);
  this.reset = function () {
    var data = [0xFE];
    I2C.write(this.handle, data);
    delay(500);
  };

  this.getTemperature = function () {
    var data = [0xF3];
    I2C.write(this.handle, data);
    delay(5000);
    var val = I2C.read(this.handle, 2);
    var temp = val[0] << 8;
    temp = temp | val[1];
    temp *= 175.72;
    temp /= 65536;
    temp -= 46.85;
    return temp;
  };

  this.getHumidity = function () {
    var data = [0xF5];
    I2C.write(this.handle, data);
    delay(5000);
    var val = I2C.read(this.handle, 2);
    var temp = val[0] << 8;
    temp = temp | val[1];
    temp *= 125;
    temp /= 65536;
    temp -= 6;
    return temp;
  };
};

module.exports = htu21d;

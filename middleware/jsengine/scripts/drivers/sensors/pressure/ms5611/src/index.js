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

var ms5611 = function (id) {
  this.handle = I2C.open(id);
  this.reset = function () {
    var data = [0x1E];
    I2C.write(this.handle, data);
    delay(500);
  };

  this.valConversion = function () {
    var data = [0x00];
    I2C.write(this.handle, data);
    var val = I2C.read(this.handle, 3);
    var temp = val[0] << 16;
    temp = temp | (val[1] << 8);
    temp = temp | val[0];
    return temp;
  };

  this.getPressure = function () {
    var data = [0x48];
    I2C.write(this.handle, data);
    delay(5000);
    var temp = this.valConversion();
    return temp;
  };

  this.getTemperature = function () {
    var data = [0x58];
    I2C.write(this.handle, data);
    delay(5000);
    var temp = this.valConversion();
    return temp;
  };
};

module.exports = ms5611;

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

var tmp102 = function (id) {
  this.handle = I2C.open(id);
  this.openFlag = 0;
  this.regRead = function (reg) {
    var data = [0x00];
    data[0] = reg;
    I2C.write(this.handle, data);
    var val = I2C.read(this.handle, 1);
    return val[0];

  };

  this.init = function () {
    var data = [0x00, 0x00];
    data[0] = 0x70;
    data[1] = 0xa0;
    I2C.write(this.handle, data);
  };

  this.getTemperature = function () {

    if (0 == this.openFlag) {
      this.init();
      this.openFlag = 1;
    }

    var data = [0x00];
    I2C.write(this.handle, data);
    var val = I2C.read(this.handle, 2);
    var temp = ((val[0] << 8) | (val[1])) >> 4;
    temp = temp * 0.0625;
    return temp;
  };
};

module.exports = tmp102;

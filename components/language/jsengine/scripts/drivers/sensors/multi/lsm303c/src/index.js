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

var lsm303c = function (id) {
  this.handle = I2C.open(id);
  this.openflag = 0;
  this.regRead = function (reg) {

    var data = [0x00];
    data[0] = reg;
    I2C.write(this.handle, data);
    var val = I2C.read(this.handle, 1);
    return val[0];

  };
  this.regWrite = function (reg, val) {
    var data = [0x00, 0x00];
    data[0] = reg;
    data[1] = val;
    I2C.write(this.handle, data);
  };

  this.init = function () {
    this.regWrite(0x20, 0x67);
  };


  this.getAcc = function () {
    var buff = [0, 0, 0, 0, 0, 0];
    var i = 0;
    var tmp = 0;
    if (0 == this.openflag) {
      this.init();
      this.openflag = 1;
    }
    for (i = 0; i < 10; i++) {
      tmp = this.regRead(0x27);
      if (tmp & 0x08) {
        break;
      }
    }
    if (tmp & 0x08) {
      buff[0] = this.regRead(0x28);
      buff[1] = this.regRead(0x29);
      buff[2] = this.regRead(0x2a);
      buff[3] = this.regRead(0x2b);
      buff[4] = this.regRead(0x2c);
      buff[5] = this.regRead(0x2d);
    }
    return buff;
  };

};

module.exports = lsm303c;

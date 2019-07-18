/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var qmc5883 = function (id) {
  this.isInited = 0;
  this.handle = I2C.open(id);
  this.statusReg = 0x06;

  this.write_one = function (addr, val) {
    var data = [0, 0];
    data[0] = addr;
    data[1] = val;
    I2C.write(this.handle, data);
  };


  this.init_config = function () {
    this.write_one(0x09, 0x0d);
    this.write_one(0x0b, 0x01);
    this.write_one(0x20, 0x40);
    this.write_one(0x21, 0x01);
  };

  this.check_ready = function () {
    var reg = [0];
    reg[0] = this.statusReg;
    I2C.write(this.handle, reg);
    var val = I2C.read(this.handle, 1);
    return val[0] & 0x01;
  };


  this.getXYZ = function () {

    var result = [-1, -1, -1];
    var reg = [0];
    if (0 == this.isInited) {
      this.init_config();
      this.isInited = 1;
    }
    reg[0] = this.statusReg;
    I2C.write(this.handle, reg);
    var status = I2C.read(this.handle, 1);
    var ready = status[0] & 0x01;

    if (0 == ready) {
      return result;
    }
    reg[0] = 0x00;
    I2C.write(this.handle, reg);
    var val = I2C.read(this.handle, 6);
    result[0] = val[1] << 8;
    result[0] = result[0] | val[0];

    result[1] = val[3] << 8;
    result[1] = result[1] | val[2];

    result[2] = val[5] << 8;
    result[2] = result[2] | val[4];
    return result;
  };

};


module.exports = qmc5883;
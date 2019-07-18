/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var lsm6dsl = function (id) {
  this.isInited = 0;
  this.handle = I2C.open(id);
  this.xlowAccReg = 0x28;
  this.ylowAccReg = 0x2a;
  this.zlowAccReg = 0x2c;
  this.xlowGyroReg = 0x22;
  this.ylowGyroReg = 0x24;
  this.zlowGyroReg = 0x26;

  this.read_one = function (addr) {
    var reg = [0];
    reg[0] = addr;
    I2C.write(this.handle, reg);
    var val = I2C.read(this.handle, 1);
    return val;
  };

  this.read_two = function (addr) {

    var low = this.read_one(addr);
    var high = this.read_one(addr + 1);
    var mem_val = high[0] << 8;
    mem_val = mem_val | low[0];
    return mem_val;
  };

  this.write_one = function (addr, val) {
    var data = [0, 0];
    data[0] = addr;
    data[1] = val;
    I2C.write(this.handle, data);
  };

  this.init_config = function () {
    /*6.6kHz*/
    this.write_one(0x10, 0xA0);
    /*6.6kHz*/
    this.write_one(0x11, 0xA0 | 0x02);
  };

  this.getAcc = function () {

    var acc = [0, 0, 0];
    if (0 == this.isInited) {
      this.init_config();
      this.isInited = 1;
    }
    acc[0] = this.read_two(this.xlowAccReg);
    acc[0] = acc[0] * 61 / 1000;
    acc[1] = this.read_two(this.ylowAccReg);
    acc[1] = acc[0] * 61 / 1000;
    acc[2] = this.read_two(this.zlowAccReg);
    acc[2] = acc[0] * 61 / 1000;
    return acc;
  };

  this.getGyro = function () {
    var gyro = [0, 0, 0];
    if (0 == this.isInited) {
      this.init_config();
      this.isInited = 1;
    }
    gyro[0] = this.read_two(this.xlowGyroReg);
    gyro[0] = gyro[0] * 70;
    gyro[1] = this.read_two(this.ylowGyroReg);
    gyro[1] = gyro[1] * 70;
    gyro[2] = this.read_two(this.zlowGyroReg);
    gyro[2] = gyro[2] * 70;
    return gyro;
  };

};

module.exports = lsm6dsl;
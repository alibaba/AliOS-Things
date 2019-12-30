/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var mmc3680kj = function (id) {

  this.isOn = 1;
  this.handle = I2C.open(id);
  this.enRead = 0;
  this.control_reg = 0x08;
  this.status_reg = 0x07;
  this.xlow_reg = 0x00;
  this.ylow_reg = 0x02;
  this.zlow_reg = 0x04;

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

  this.getmemsic = function () {
    var val = [-1, -1, -1];
    var i = 0;
    var status = 0;
    if (0 == this.enRead) {

      this.write_one(this.control_reg, 0x08);
      this.write_one(this.control_reg, 0x01);
      this.enRead = 1;
    }

    for (i = 0; i < 10; i++) {
      status = this.read_one(this.status_reg);
      if (0x01 == (status[0] & 0x01)) {
        break;
      }
    }

    if (0x01 != (status[0] & 0x01)) {
      return val;
    }

    var x_value = this.read_two(this.xlow_reg);
    var y_value = this.read_two(this.ylow_reg);
    var z_value = this.read_two(this.zlow_reg);

    y_value = y_value - z_value + 32768;
    z_value = y_value + z_value - 32768;
    x_value = (z_value - 32768) * 1000000 / 1024;
    y_value = (y_value - 32768) * 1000000 / 1024;
    z_value = (z_value - 32768) * 1350000 / 1024;

    val[0] = x_value;
    val[1] = y_value;
    val[2] = z_value;
    this.enRead = 0;

    return val;
  };

};

module.exports = mmc3680kj;





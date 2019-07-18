/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var bmp280 = function (id) {

  this.isInited = 0;
  this.handle = I2C.open('bmp280');
  this.lowTempReg = 0xfa;
  this.lowPressReg = 0xf7;
  this.dig_T1 = 0;
  this.dig_T2 = 0;
  this.dig_T3 = 0;
  this.dig_P1 = 0;
  this.dig_P2 = 0;
  this.dig_P3 = 0;
  this.dig_P4 = 0;
  this.dig_P5 = 0;
  this.dig_P6 = 0;
  this.dig_P7 = 0;
  this.dig_P8 = 0;
  this.dig_P9 = 0;
  this.t_fine = 0;
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
    var mem_val = low[0] << 8;
    mem_val = mem_val | high[0];
    return mem_val;
  };

  this.read_three = function (addr) {

    var msb_value = this.read_one(addr);
    var lsb_value = this.read_one(addr + 1);
    var xlsb_value = this.read_one(addr + 2);
    var val_high = msb_value[0] << 12;
    var val_mid = lsb_value[0] << 4;
    var val_low = xlsb_value[0] >> 4;
    var mem_val = val_high | val_mid | val_low;
    return mem_val;
  };

  this.write_one = function (addr, val) {
    var data = [0, 0];
    data[0] = addr;
    data[1] = val;
    I2C.write(this.handle, data);
  };

  this.init_config = function () {

    this.write_one(0xe0, 0xb6);
    this.write_one(0xf4, 0x27);
    this.write_one(0xf5, 0x20);
    this.dig_T1 = this.read_two(0x88);
    this.dig_T2 = this.read_two(0x8A);
    this.dig_T3 = this.read_two(0x8C);
    this.dig_P1 = this.read_two(0x8E);
    this.dig_P2 = this.read_two(0x90);
    this.dig_P3 = this.read_two(0x92);
    this.dig_P4 = this.read_two(0x94);
    this.dig_P5 = this.read_two(0x96);
    this.dig_P6 = this.read_two(0x98);
    this.dig_P7 = this.read_two(0x9A);
    this.dig_P8 = this.read_two(0x9C);
    this.dig_P9 = this.read_two(0x9E);

  };


  this.getPressure = function () {

    if (0 == this.isInited) {
      this.init_config();
      this.isInited = 1;
    }
    var press_value = this.read_three(this.lowPressReg);

    return press_value;
  };

  this.getTemperature = function () {

    if (0 == this.isInited) {
      this.init_config();
      this.isInited = 1;
    }
    var temp_value = this.read_three(this.lowTempReg);

    return temp_value;
  };


};

module.exports = bmp280;
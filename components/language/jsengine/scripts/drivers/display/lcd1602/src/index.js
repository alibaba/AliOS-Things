/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var lcd1602 = function (id) {

  this.handle = I2C.open(id);
  this.backlight = 0x08;
  this.control = 0x04;
  this.mode = 0x02;
  this.init_flag = 0;
  var self = this;
  this.write_one = function (addr, val) {
    var data = [0, 0];
    data[0] = addr;
    data[1] = val;
    I2C.write(self.handle, data);
  };

  this.write_to_pcf8574 = function (val) {
    var data = [0];
    data[0] = val | (self.backlight);
    I2C.write(self.handle, data);
  };

  this.write_enable = function (val) {
    self.write_to_pcf8574(val | 0x04);
    self.write_to_pcf8574(val | (0xFB));
  };

  this.write_wrap = function (val) {
    self.write_to_pcf8574(val);
    self.write_enable(val);
  };

  this.write = function (val, flag) {
    var data = (val & 0xF0) | flag;
    self.write_wrap(data);
    data = (val & 0x0F) << 4;
    data = data | flag;
    self.write_wrap(data);
  };

  this.send_cmd = function (cmd) {
    self.write(cmd, 0x00);
  };

  this.send_data = function (data) {
    self.write(data, 0x01);
  };

  this.clear = function () {
    self.send_cmd(0x01);
  };

  this.home = function () {
    self.send_cmd(0x02);
  };

  this.set_cursor = function (col, row) {

    var offset = [0x00, 0x40, 0x14, 0x54];
    if (row > 2 || col > 40) {
      return -1
    }
    var data = col + offset[row]
    data = data | 0x80
    self.send_cmd(data);
    return 0
  };

  this.show_char = function (col, row, c) {
    var ret = 0;
    ret = self.set_cursor(col, row);
    if (0 == ret) {
      self.send_data(c);
    }
  };

  this.init_lcd = function () {
    if (self.init_flag) {
      return
    }
    self.init_flag = 1;
    self.write_to_pcf8574(0);
    self.write_wrap(0x03 << 4);
    self.write_wrap(0x03 << 4);
    self.write_wrap(0x03 << 4);
    self.write_wrap(0x02 << 4);
    self.send_cmd(0x20 | 0x08);
    self.send_cmd(0x08 | (self.control));
    self.clear();
    self.send_cmd(0x04 | (self.mode));
    self.home();
  };

};

module.exports = lcd1602;





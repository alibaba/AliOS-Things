/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var show_buff = [0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92];
var show_sect = [0, 1, 2, 4, 8];

var tube595 = function (sclk_ID, rclk_ID, io_ID) {

  this.handle_sclk = GPIO.open(sclk_ID);
  this.handle_rclk = GPIO.open(rclk_ID);
  this.handle_io = GPIO.open(io_ID);
  this.show = function (data) {
    var i = 0;
    for (i = 8; i > 0; i--) {
      if (data & 0x80) {
        GPIO.write(this.handle_io, 1);
      } else {
        GPIO.write(this.handle_io, 0);
      }
      data = data << 1;
      GPIO.write(this.handle_sclk, 0);
      GPIO.write(this.handle_sclk, 1);
    }
  };

  this.set = function (num) {
    if (num > 5) {
      console.log('wrong num!');
      return;
    }
    var data = show_buff[num];
    var sect = show_sect[num];
    this.show(data);
    this.show(sect);
    GPIO.write(this.handle_rclk, 0);
    GPIO.write(this.handle_rclk, 1);
  };

};

module.exports = tube595;



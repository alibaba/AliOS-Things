/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
// 人体红外传感器
// 将 hcsr501 的OUT 管脚直接接到GPIO管脚，hcrs501 需要一个GPIO脚

var hcsr501 = function (id) {
  this.handle = GPIO.open(id);
  this.cb = null;

  this.check = function () {
    var val = GPIO.read(this.handle);
    return val;
  };
};

module.exports = hcsr501;
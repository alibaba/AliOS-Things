/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var rf13 = function (id) {
  this.handle = ADC.open(id);
  this.get_sound = function () {
    return ADC.read(this.handle);
  };
};

module.exports = rf13;
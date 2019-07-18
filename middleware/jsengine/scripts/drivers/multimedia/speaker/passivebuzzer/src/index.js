/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var passivebuzzer = function (id) {

  console.log('id:' + id);
  this.handle = DAC.open(id);
  var self = this;
  this.once = function () {
    var i = 0;
    for (i = 255; i > 0; i--) {
      DAC.setVol(self.handle, i);
      DAC.setVol(self.handle, 255 - i);
      DAC.setVol(self.handle, i);
    }
  };
};

module.exports = passivebuzzer;


/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var lm358 = function(id) {
  this.handle = DAC.open(id);
  this.setVal = function(val) {
    DAC.setVol(this.handle,val);
  };
  this.getVal = function() {
    var val = DAC.getVol(this.handle);
    return val;
  };
};

module.exports = lm358;
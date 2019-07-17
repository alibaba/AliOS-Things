/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var temt6000 = function(id) {
  this.handle = ADC.open(id);
  this.getVal = function() {
    var i = 0;
    var val = 0;
    for (i=0; i<10; i++){
      val += ADC.read(this.handle);
    }
    val = val /10;
    return val;
  };
};

module.exports = temt6000;

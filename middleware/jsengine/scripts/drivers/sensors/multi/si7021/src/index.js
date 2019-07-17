/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var delay = function(val){
  var i = 0;
  var tmp = 0;
  for(i=0;i<val;i++){
    tmp = 0;
  }
};

var si7021 = function(id) {
  this.handle = I2C.open(id);
  this.openFlag = 0;
  this.regRead = function(reg){
    var data = [0x00];
    data[0] = reg;
    I2C.write(this.handle,data);
    var val = I2C.read(this.handle,1);
    return val[0];

  };

  this.getHumi = function() {

    var data = [0xE5];
    I2C.write(this.handle,data);
    var val = I2C.read(this.handle,2);
    var temp = (val[0]<<8) | (val[1]);
    temp = 125.0*temp/65536 - 6;
    return temp;
  };

  this.getTemperature = function() {

    var data = [0xE0];
    I2C.write(this.handle,data);
    var val = I2C.read(this.handle,2);
    var temp = (val[0]<<8) | (val[1]);
    temp = 175.72*temp/65536 - 46.85;
    return temp;
  };
};

module.exports = si7021;

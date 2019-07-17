/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var at24c32 = function(id) {

  this.handle = I2C.open(id);
  this.read_one = function(addr){
    var reg=[0,0];
    reg[0] = addr>>8;
    reg[1] = addr & 0xFF;
    I2C.write(this.handle,reg);
    var val = I2C.read(this.handle,1); 
    return val[0];
  };

  this.write_one = function(addr,val){
      var data = [0,0,0];
      data[0] = addr>>8;
      data[1] = addr & 0xFF;
      data[2]=val; 
      I2C.write(this.handle,data);
  };

};


module.exports = at24c32;
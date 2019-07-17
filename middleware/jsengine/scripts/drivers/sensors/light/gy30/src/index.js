/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var gy30 = function(id) {

  this.handle = I2C.open(id);
  
  this.read_two = function(){
    var val = I2C.read(this.handle,2); 
    var result = val[0] << 8;
    result = result | val[1];
    return result;
  };

  this.write_one = function(val){
      var data = [0];
      data[0]=val; 
      I2C.write(this.handle,data);
  };

  this.init_config = function(){
      this.write_one(0x01);
      this.write_one(0x10);
  };

  this.get_light = function(){

      if(0 == this.isInited){
          this.init_config();
          this.isInited = 1;   
      }
      var val = this.read_two();

      return val;
  };

};


module.exports = gy30;
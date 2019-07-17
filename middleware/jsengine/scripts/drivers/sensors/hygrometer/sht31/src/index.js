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

var sht31 = function(id) {
  this.handle = I2C.open(id);
  this.openflag = 0;
  this.regRead = function(reg){

    var data = [0x00];
    data[0] = reg;
    I2C.write(this.handle,data);
    var val = I2C.read(this.handle,1);
    return val[0];

  };
  this.regWrite = function(reg,val){
    var data = [0x00,0x00];
    data[0] = reg;
    data[1] = val;
    I2C.write(this.handle,data);
  };
  this.getHumTem = function(){

    var data = [0,0];
    var cmd=[0x2C,0x06]; 
    I2C.write(this.handle,cmd);
    delay(10);
    var val = I2C.read(this.handle,6);
    data[0] = (val[0]<<8) | (val[1]);  //tem
    data[1] = (val[3]<<8) | (val[4]);  //hum
    data[0] *= 175;
    data[0] /= 0xffff;
    data[0] = -45 + data[0];
    data[0] = data[0];
  
    data[1] *= 100;
    data[1] /= 0xFFFF;
  

    return data;
  };

};

module.exports = sht31;
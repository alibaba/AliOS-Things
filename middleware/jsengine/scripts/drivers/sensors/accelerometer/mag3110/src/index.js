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

var mag3110 = function(id) {
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

  this.init = function() {
    this.regWrite(0x11,0x80); 
    this.regWrite(0x10,0xc9); 
  };

  
  this.getAcc = function(){
    var buff = [0,0,0,0,0,0];
    var i = 0;
    var tmp = 0;
    if(0 == this.openflag){
      this.init();
      this.openflag = 1;
    }
    for(i=0;i<20;i++){
      tmp = this.regRead(0x00);
      if(tmp&0x80){
        break;
      }
      delay(100);
    }
    if(tmp&0x80){
      buff[0] =  this.regRead(0x01); 
      buff[1] =  this.regRead(0x02); 
      buff[2] =  this.regRead(0x03); 
      buff[3] =  this.regRead(0x04); 
      buff[4] =  this.regRead(0x05); 
      buff[5] =  this.regRead(0x06); 
    }
    return buff;
  };

};

module.exports = mag3110;
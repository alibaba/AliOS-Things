/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var relay = function(id) {
  this.level = 0;
  this.handle = GPIO.open(id);
  this.set = function(val){
    if(val == this.level){
        return;
    }
    GPIO.write(this.handle, val);
    this.level = val;
  };

  this.get = function(){
      return this.level;
  };
};



module.exports = relay;
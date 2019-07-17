/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var isd1820 = function(play_id,record_id) {

  this.playHandle   = GPIO.open(play_id);
  this.recordHandle = GPIO.open(record_id);
  this.startRecord = function(){
    GPIO.write(this.recordHandle,1);
  };

  this.stopRecord = function(){
      GPIO.write(this.recordHandle,0);
  };

  this.startPlay = function(){
      GPIO.write(this.playHandle,0);
      var i = 0;
      var tmp  = 0;
      for(i=0;i<500;i++){
          tmp = 0;
      }
      GPIO.write(this.playHandle,1);
  };

};

module.exports = isd1820;


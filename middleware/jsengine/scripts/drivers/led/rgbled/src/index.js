/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var rgbled = function(rled,gled,bled){

  this.handle1 = GPIO.open(rled);
  this.handle2 = GPIO.open(gled);
  this.handle3 = GPIO.open(bled);  
  this.red = function() {
    GPIO.write(this.handle1, 0);
    GPIO.write(this.handle2, 1); 
    GPIO.write(this.handle3, 1); 
  };
  this.green = function() {
    GPIO.write(this.handle2, 0);
    GPIO.write(this.handle1, 1); 
    GPIO.write(this.handle3, 1);       
  }; 
  this.blue = function() {
    GPIO.write(this.handle3, 0);
    GPIO.write(this.handle1, 1); 
    GPIO.write(this.handle2, 1);  
  };   
  this.set = function(rstate, gstate, bstate) {
    GPIO.write(this.handle1, rstate); 
    GPIO.write(this.handle2, gstate);
    GPIO.write(this.handle3, bstate);   
  };

};


module.exports = rgbled;
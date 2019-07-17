/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var matrixkeypad = function(c1_id,c2_id,c3_id,c4_id,r1_id,r2_id,r3_id,r4_id) {

  this.C4Handle   = GPIO.open(c4_id);
  this.C3Handle   = GPIO.open(c3_id);
  this.C2Handle   = GPIO.open(c2_id);
  this.C1Handle   = GPIO.open(c1_id);
  this.R1Handle   = GPIO.open(r1_id);
  this.R2Handle   = GPIO.open(r2_id);
  this.R3Handle   = GPIO.open(r3_id);
  this.R4Handle   = GPIO.open(r4_id);
  this.keycb = null;

  this.reset = function(){
    GPIO.write(this.R1Handle,0);
    GPIO.write(this.R2Handle,0);
    GPIO.write(this.R3Handle,0);
    GPIO.write(this.R4Handle,0);
  };


  this.init = function(cb){
      this.reset();
      this.keycb = cb;
  };

  this.setCval = function(index){

      if(0 == index){
          GPIO.write(this.R1Handle,1);
          GPIO.write(this.R2Handle,0);
          GPIO.write(this.R3Handle,0);
          GPIO.write(this.R4Handle,0);
      }else if(1 == index){
          GPIO.write(this.R1Handle,0);
          GPIO.write(this.R2Handle,1);
          GPIO.write(this.R3Handle,0);
          GPIO.write(this.R4Handle,0);
      }else if(2 == index){
          GPIO.write(this.R1Handle,0);
          GPIO.write(this.R2Handle,0);
          GPIO.write(this.R3Handle,1);
          GPIO.write(this.R4Handle,0);     
      }else{
          GPIO.write(this.R1Handle,0);
          GPIO.write(this.R2Handle,0);
          GPIO.write(this.R3Handle,0);
          GPIO.write(this.R4Handle,1);  

      }

  };

  this.measure = function(handle){

      var  i = 0;
      var  val = 0;
      var  ret = -1;
      for(i=0;i<4;i++){

          this.setCval(i);
          val = GPIO.read(handle);
          if(1 == val){
              ret = i;
              break;
          }
      }
      return ret;
  };


  this.scan = function(){
      var cval = -1;
      var rval = -1;
      var i = 0;
      var  keymap = [this.C4Handle,this.C3Handle,this.C2Handle,this.C1Handle];
      for(i=0;i<4;i++){
          cval = GPIO.read(keymap[i]);
          if(0 == cval){
              rval = this.measure(keymap[i]);
              if(-1 != rval){
                  if(null != this.keycb){
                    this.keycb(rval,i);
                  }
                  this.reset();
                  break;
              }
          }
      }
  };

};




module.exports = matrixkeypad;


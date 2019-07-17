/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var keyFun = nil;
var handlekey0 = nil;
var handlekey1 = nil;
var key0flag = 0;
var key1flag = 0;

var touchkey = function(tkey0_id,tkey1_id) {
    handlekey0 = ADC.open(tkey0_id);
    handlekey1 = ADC.open(tkey1_id);
    this.check = function(){
      var key0 = ADC.read(handlekey0);
      var key1 = ADC.read(handlekey1);
      if(key0 < 15){
          key0flag = key0flag+1;    
      }
      
      if (key0flag > 3 && key0 > 16){
          key0flag = 0;
          process.nextTick(function(){
              if(keyFun){
                  keyFun('key0');
              }
          });
      }else if (key0 > 16){
          key0flag = 0;
      }
  
      if(key1 < 15){
          key1flag = key1flag + 1;    
      }
  
      if (key1flag > 3 && key1>16){
          key1flag = 0;
          process.nextTick(function(){
              if(keyFun){
                  keyFun('key1');
              }
          });
      }else if(key1>16){
          key1flag = 0;   
      }
  };
  
  this.on = function(keycb){
      keyFun =   keycb;
      setInterval(this.check, 50); 
  };

};

module.exports = touchkey;

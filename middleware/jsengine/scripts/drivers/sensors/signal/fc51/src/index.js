/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

var fc51 = function(id) {
    this.handle = GPIO.open(id);
    this.GetFlags = function(){
        var val;
        val = GPIO.read(this.handle);
        return val;
    }; 
};

module.exports = fc51;





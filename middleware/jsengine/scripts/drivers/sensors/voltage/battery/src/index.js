/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
var battery = function(id) {

    this.adc_handle = ADC.open(id);
    this.bits = 10;
    this.verf = 3.3;

    this.config = function(verf,bits) {
        this.bits = bits;
        this.verf = verf;
    }; 

    this.read_voltage = function() {
        var sum = 0.0;
        var i = 0;
        for(i=0;i<5;i++){
            sum += ADC.read(this.adc_handle);
        }
       return ((sum/5.0)/(1<this.bits))*3.3;
    };
};
  
module.exports = battery;



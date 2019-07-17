/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

const min_duty = 500.0;
const mid_duty = 1500.0;
const max_duty = 2500.0;
const cycle = 20000.0;
const diff_angle = 90.0;
const diff_duty = 1000.0;

var ds3119 = function(pwm_id) {

    this.pwm_handle = PWM.start(pwm_id);
 
    this.min_angle = -90;
    this.mid_angle = 0;
    this.max_angle = 90;
    this.cur_angle = 0;

    this.calculate_duty = function(angle){
    
        if(angle < this.min_angle) {
            return (-1);
        }
    
        if(angle > this.max_angle) {
            return (-1);
        }
        var tmp = angle;
        if(tmp < 0){
            tmp = 0- tmp;     
        }
        var duty = (tmp/diff_angle)*diff_duty;
        if(angle < 0){
            duty = mid_duty - duty;
        }else {
            duty = mid_duty + duty;
        }
        return duty;
    
    };

    this.config = function(min,mid,max) {
        this.min_angle = min;
        this.mid_angle = mid;
        this.max_angle = max;
    };


    this.reset = function() {
        var pwm_value = mid_duty/cycle * 100.0;
        PWM.setDuty(this.pwm_handle,pwm_value);
        this.cur_angle = 0;

    };

    this.left_turn = function(angle) {

        var cur_angle = this.cur_angle - angle;
        if (cur_angle < this.min_angle) {
            cur_angle  = this.min_angle;    
        }
        var duty_val = this.calculate_duty(cur_angle);
        if(duty_val < 0){
            return -1;
        }
        var pwm_value = duty_val/cycle * 100.0;
        PWM.setDuty(this.pwm_handle,pwm_value);
        this.cur_angle = cur_angle;
        return 0;

    };

    this.right_turn = function(angle) {

        var cur_angle = this.cur_angle + angle;
        if (cur_angle > this.max_angle) {
            cur_angle  = this.max_angle;    
        }
        var duty_val = this.calculate_duty(cur_angle);
        if(duty_val < 0){
            return -1;
        }
        var pwm_value = duty_val/cycle * 100.0;
        PWM.setDuty(this.pwm_handle,pwm_value);
        this.cur_angle = cur_angle;
        return 0;
    };

};
  
module.exports = ds3119;




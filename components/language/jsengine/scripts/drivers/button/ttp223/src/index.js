/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

//ttp223：touch pad detector
// 需要一个GPIO管脚，目录中 driver.json 对应的是 ESP32 芯片，其他芯片则需要根据datasheet来配置对应的GPIO管脚
// 本驱动采用的是边缘触发模式，也可以直接读GPIO的管脚的电平，通过轮询读取状态

var ttp223 = function (id) {
  //直接连接MCU GPIO管脚，通过检测 GPIO 的电平变化来	判断状态
  this.handle = GPIO.open(id);

  //检测边沿触发，而不是检测高低电平，可以不管TTP223的工作模式！
  this.on = function (edge, cb) {
    GPIO.on(this.handle, edge, cb);
  }
};

module.exports = ttp223;

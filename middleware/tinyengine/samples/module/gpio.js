// led每秒闪烁
var dev = {port: 12, config: Gpio.OUTPUT_PUSH_PULL};
Gpio.open(dev);

var status = 0;
setInterval(function() {
  Gpio.write(12, (status++)%2);
}, 1000);

// 监听按键
var dev = {port: 1, config: Gpio.OUTPUT_PUSH_PULL, trigger: Gpio.IRQ_TRIGGER_FALLING_EDGE};
Gpio.enableIrq(dev, function() {
  console.log('key down');
});
/* 加载 timer 模块 */
import * as timer from 'timer'

/* 创建 timer 实例 */
var timer0 = timer.open({
  id: 'TIMER0'
});

console.log('HardWare Timer Test');

/* 定义用来统计触发次数的变量 */
var count = 0;

/* 定义定时到期回调函数，单次触发打印一行字符串及触发次数 */
function timeout_cb() {
    console.log('Timer0 Timeout count' + count);
    count++;
}

/* 设置单次定时，定时时间5000000us，回调函数打印一行字符串 */
timer0.setTimeout(timeout_cb, 5000000);

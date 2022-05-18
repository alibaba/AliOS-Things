
var rtc = require('rtc');

rtc.start();

var current_time = rtc.getTime();
console.log('rtc1: current time is ' + current_time);

var my_date = new Date();
my_date.setFullYear(2008,7,9);

rtc.setTime(my_date);

console.log('Date is ' + my_date.toUTCString());

current_time = rtc.getTime();
console.log('rtc2: current time is ' + current_time);

rtc.close();
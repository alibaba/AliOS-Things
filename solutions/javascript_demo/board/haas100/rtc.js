import * as rtc from 'rtc'

rtc.start();

var current_time = rtc.getTime();
console.log('rtc1: current time is ' + current_time);

var my_date = new Date();
my_date.setFullYear(2008,7,9);

console.log('rtc1: time.getYear() ' + my_date.getYear());
console.log('rtc1: time.getMonth() ' + my_date.getMonth());
console.log('rtc1: time.getDate() ' + my_date.getDate());
console.log('rtc1: time.getHours() ' + my_date.getHours());
console.log('rtc1: time.getMinutes() ' + my_date.getMinutes());
console.log('rtc1: time.getSeconds() ' + my_date.getSeconds());

rtc.setTime(my_date);

console.log('Date is ' + my_date.toUTCString());

current_time = rtc.getTime();
console.log('rtc2: current time is ' + current_time);

rtc.close();

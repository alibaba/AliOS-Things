'use strict';

function start() {
    __native.RTC.open();
}

function setTime(time) {
    if (!time) {
        throw new Error('params is invalid');
    }

    if (!(time instanceof Date)) {
        throw new Error('params is invalid');
    }

    var date = {
        year: time.getYear(),
        month: time.getMonth(),
        day: time.getDate(),
        hour: time.getHours(),
        minute: time.getMinutes(),
        second: time.getSeconds()
    }
    __native.RTC.setTime(date);
}

function getTime() {
    var time = __native.RTC.getTime();
    console.log(time);
    return new Date(parseInt(time.year) + 1900, parseInt(time.month), parseInt(time.day), parseInt(time.hour), parseInt(time.minute), parseInt(time.second));
}

function close() {
    __native.RTC.close();
}

module.exports = {
    start,
    setTime,
    getTime,
    close
}
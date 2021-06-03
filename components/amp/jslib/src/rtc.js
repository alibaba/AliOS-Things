import * as RTC from 'RTC'

export function start() {
    RTC.open();
}

export function setTime(time) {
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
    RTC.setTime(date);
}

export function getTime() {
    var time = RTC.getTime();
    console.log(time);
    return new Date(parseInt(time.year) + 1900, parseInt(time.month), parseInt(time.day), parseInt(time.hour), parseInt(time.minute), parseInt(time.second));
}

export function close() {
    RTC.close();
}

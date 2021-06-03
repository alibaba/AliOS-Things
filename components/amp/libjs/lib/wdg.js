'use strict';

function start(timeout) {
    if (!timeout) {
        throw new Error('params is invalid');
    }
    __native.WDG.start(timeout);
}

function feed() {
    __native.WDG.feed();
}

function stop() {
    __native.WDG.stop();
}

module.exports = {
    start,
    feed,
    stop
}
'use strict';

const EventEmitter = require('events');

module.exports = new class UTILS_PM extends EventEmitter {
    constructor(){
        super();
        this._onPwrkey();
    }

    _onPwrkey() {
        __native.PM.onPwrkey(function(state) {
            this.emit('powerKey', state);
        }.bind(this));
    };

    setAutosleepMode(mode) {
        var ret = __native.PM.setAutosleepMode(mode);
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
    }

    getAutosleepMode() {
        var ret = __native.PM.getAutosleepMode();
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
    }

    sleep() {
        var ret = __native.PM.sleep();
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
    }

    powerReset() {
        var ret = __native.PM.powerReset();
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
    }

    powerDown() {
        var ret = __native.PM.powerDown();
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
    }

    wakelockLock() {
        var ret = __native.PM.wakelockLock();
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
    }

    wakelockUnlock() {
        var ret = __native.PM.wakelockUnlock();
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
    }

    wakelockTimedlock(timeMs) {
        var ret = __native.PM.wakelockTimedlock(timeMs);
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
    }
}
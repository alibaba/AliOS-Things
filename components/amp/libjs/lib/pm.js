'use strict';

const EventEmitter = require('events');

module.exports = new class UTILS_PM extends EventEmitter {
    constructor(){
        super();
        this._onPwrkey();
    }

    _onPwrkey() {
        __native.PM.on_pwrkey(function(state) {
            this.emit('powerKey', state);
        }.bind(this));
    };

    setAutosleepMode(mode) {
        var ret = __native.PM.set_autosleep_mode(mode);
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
        this.emit('lowpower', ret);
    }

    getAutosleepMode() {
        var ret = __native.PM.get_autosleep_mode();
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
        this.emit('lowpower', ret);
    }

    sleep() {
        var ret = __native.PM.sleep();
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
        this.emit('lowpower', ret);
    }

    wakelockLock() {
        var ret = __native.PM.wakelock_lock();
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
        this.emit('lowpower', ret);
    }

    wakelockUnlock() {
        var ret = __native.PM.wakelock_unlock();
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
        this.emit('lowpower', ret);
    }

    wakelockTimedlock(timeMs) {
        var ret = __native.PM.wakelock_timedlock(timeMs);
        if (ret < 0) {
            this.emit('error', ret);
            return;
        }
        this.emit('lowpower', ret);
    }
}
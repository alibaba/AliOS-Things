'use strict';

class HW_TIMER {
    constructor(options) {
        if (!options || !options.id) {
            throw new Error('options is invalid');
        }

        this.options = {
            id: options.id
        };

        this.success = options.success || function(){};
        this.fail = options.fail || function(){};
        this._open();
    }

    _open() {
        this.timerInstance = __native.TIMER.open(this.options.id);
        if (!this.timerInstance) {
            this.fail();
            return;
        }
        this.success();
    }

    setTimeout(cb, timeout) {
        if (!this.timerInstance) {
            throw new Error("timer not init");
        }
        var ret = __native.TIMER.setTimeout(this.timerInstance, cb, timeout);
        if (ret != 0) {
            throw new Error("set time out failed");
        }
    }

    clearTimeout() {
        if (!this.timerInstance) {
            throw new Error("timer not init");
        }
        var ret = __native.TIMER.clearTimeout(this.timerInstance);
        if (ret != 0) {
            throw new Error("clear time out failed");
        }
    }

    setInterval(cb, timeout) {
        if (!this.timerInstance) {
            throw new Error("timer not init");
        }
        var ret = __native.TIMER.setInterval(this.timerInstance, cb, timeout);
        if (ret != 0) {
            throw new Error("set interval failed");
        }
    }

    clearInterval() {
        if (!this.timerInstance) {
            throw new Error("timer not init");
        }
        var ret = __native.TIMER.clearInterval(this.timerInstance);
        if (ret != 0) {
            throw new Error("clear interval failed");
        }
    };

    close() {
        if (!this.timerInstance) {
            throw new Error("timer not init");
        }
        __native.TIMER.close(this.timerInstance);
    };
}

function open(options) {
    return new HW_TIMER(options);
}

module.exports = {
    open,
}

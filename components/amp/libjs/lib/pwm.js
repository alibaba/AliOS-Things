'use strict';

class HW_PWM {
    constructor(options) {
        if (!options || !options.id) {
            throw new Error("options is invalid");
        }
        this.options = {
            id: options.id
        };

        this.success = options.success || function(){};
        this.fail = options.fail || function(){};
        this._open();
    }

    _open() {
        this.pwmInstance = __native.PWM.open(this.options.id);
        if (!this.pwmInstance) {
            this.fail();
            return;
        }
        this.success();
    }

    set(options) {
        if (!this.pwmInstance || !options) {
            throw new Error("pwm not init or params is invalid");
        }
        __native.PWM.setConfig(this.pwmInstance, options);
    };

    get() {
        if (!this.pwmInstance) {
            throw new Error("pwm not init");
        }
        return __native.PWM.getConfig(this.pwmInstance);
    };

    close() {
        if (!this.pwmInstance) {
            throw new Error("pwm not init");
        }
        __native.PWM.close(this.pwmInstance);
    };
}

function open(options) {
    return new HW_PWM(options);
}

module.exports = {
    open,
}
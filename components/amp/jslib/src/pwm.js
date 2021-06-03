import * as PWM from 'PWM'

class HW_PWM {
    constructor(options) {
        if (!options || !options.id) {
            throw new Error("options is invalid");
        }
        console.log('pwm: pwm constructor start ' + options.id)
        this.options = {
            id: options.id
        };

        this.success = options.success || function(){};
        this.fail = options.fail || function(){};
        this._open();
    }

    _open() {
        console.log('pwm: pwm test start ' + this.options.id)
        this.pwmInstance = PWM.open(this.options.id);
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
        this.pwmInstance.setConfig(options);
    };

    get() {
        if (!this.pwmInstance) {
            throw new Error("pwm not init");
        }
        return this.pwmInstance.getConfig();
    };

    close() {
        if (!this.pwmInstance) {
            throw new Error("pwm not init");
        }
        this.pwmInstance.close();
    };
}

export function open(options) {
    return new HW_PWM(options);
}

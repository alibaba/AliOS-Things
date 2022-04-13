'use strict';

class HW_GPIO {
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
        this.gpioInstance = __native.GPIO.open(this.options.id);
        if (!this.gpioInstance) {
            this.fail();
            return;
        }
        this.success();
    }

    writeValue(level) {
        if (!this.gpioInstance) {
            throw new Error("gpio not init");
        }
        __native.GPIO.write(this.gpioInstance, level);
    }

    toggle() {
        if (!this.gpioInstance) {
            throw new Error("gpio not init");
        }        
        __native.GPIO.toggle(this.gpioInstance);
    }

    onIRQ(options) {
        if (!this.gpioInstance || !options || !options.cb) {
            throw new Error("gpio not init or params is invalid");
        }        
        __native.GPIO.on(this.gpioInstance, options.cb);
    }

    readValue() {
        if (!this.gpioInstance) {
            throw new Error("gpio not init");
        }
        return __native.GPIO.read(this.gpioInstance);
    };

    close() {
        if (!this.gpioInstance) {
            throw new Error("gpio not init");
        }
        __native.GPIO.close(this.gpioInstance);
    };
}

function open(options) {
    return new HW_GPIO(options);
}

module.exports = {
    open,
}
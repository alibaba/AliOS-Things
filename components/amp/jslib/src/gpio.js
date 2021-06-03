import * as GPIO from 'GPIO'

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
        this.gpioInstance = GPIO.open(this.options.id);
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
        this.gpioInstance.write(level);
    }

    toggle() {
        if (!this.gpioInstance) {
            throw new Error("gpio not init");
        }
        this.gpioInstance.toggle();
    }

    onIRQ(options) {
        if (!this.gpioInstance || !options || !options.cb) {
            throw new Error("gpio not init or params is invalid");
        }
        this.gpioInstance.on(options.cb);
    }

    readValue() {
        if (!this.gpioInstance) {
            throw new Error("gpio not init");
        }
        return this.gpioInstance.read();
    };

    close() {
        if (!this.gpioInstance) {
            throw new Error("gpio not init");
        }
        this.gpioInstance.close();
    };
}

export function open(options) {
    return new HW_GPIO(options);
}



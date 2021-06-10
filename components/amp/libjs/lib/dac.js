'use strict';

class HW_DAC {
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
        this.dacInstance = __native.DAC.open(this.options.id);
        if (!this.dacInstance) {
            this.fail();
            return;
        }
        this.success();
    }

    readValue() {
        if (!this.dacInstance) {
            throw new Error('dac not init');
        }
        return __native.DAC.getVol(this.dacInstance);
    };

    writeValue(value) {
        if (!this.dacInstance || !value) {
            throw new Error('dac not init or params is invalid');
        }
        __native.DAC.setVol(this.dacInstance, value);
    };

    close() {
        if (!this.dacInstance) {
            throw new Error('dac not init');
        }
        __native.DAC.close(this.dacInstance);
    };
}

function open(options) {
    return new HW_DAC(options);
}

module.exports = {
    open,
}
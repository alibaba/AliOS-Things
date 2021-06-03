import * as DAC from 'DAC'

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
        this.dacInstance = DAC.open(this.options.id);
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
        return this.dacInstance.getVol();
    };

    writeValue(value) {
        if (!this.dacInstance || !value) {
            throw new Error('dac not init or params is invalid');
        }
        this.dacInstance.setVol(value);
    };

    close() {
        if (!this.dacInstance) {
            throw new Error('dac not init');
        }
        this.dacInstance.close();
    };
}

export function open(options) {
    return new HW_DAC(options);
}

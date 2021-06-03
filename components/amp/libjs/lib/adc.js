'use strict';

class HW_ADC {
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
        this.adcInstance = __native.ADC.open(this.options.id);
        if (!this.adcInstance) {
            this.fail();
            return;
        }
        this.success();
    }

    readValue() {
        if (!this.adcInstance) {
            throw new Error('adc not init');
        }

        return __native.ADC.read(this.adcInstance);
    };

    close() {
        if (!this.adcInstance) {
            throw new Error('adc not init');
        }

        __native.ADC.close(this.adcInstance);
    };
}

function open(options) {
    return new HW_ADC(options);
}

module.exports = {
    open,
}
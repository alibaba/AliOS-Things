import * as ADC from 'ADC'

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
        this.adcInstance = ADC.open(this.options.id);
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

        return this.adcInstance.read();
    };

    close() {
        if (!this.adcInstance) {
            throw new Error('adc not init');
        }

        this.adcInstance.close();
    };
}

export function open(options) {
    return new HW_ADC(options);
}

'use strict';

const EventEmitter = require('events');

class HW_UART extends EventEmitter{
    constructor(options) {
        super();

        if (!options || !options.id) {
            throw new Error("options is invalid");
        }
        this.options = {
            id: options.id
        };

        this.success = options.success || function(){};
        this.fail = options.fail || function(){};
        this._open();
        this._onData();
    }

    _open() {
        this.uartInstance = __native.UART.open(this.options.id);
        if (this.uartInstance === null) {
            this.fail();
            return;
        }
        this.success();
    }

    write(data) {
        if (this.uartInstance === null || !data) {
            throw new Error("uart not init");
        }        
        __native.UART.write(this.uartInstance, data);
    }

    read() {
        if (this.uartInstance === null) {
        throw new Error("uart not init");
        }
        return __native.UART.read(this.uartInstance);
    };

    off() {
        if (this.uartInstance === null) {
            throw new Error("uart not init");
        }
        this.removeAllListeners('data');
    }
    _onData() {
        if (this.uartInstance === null) {
            throw new Error("uart not init");
        }
        __native.UART.on(this.uartInstance, function(len, data){
            this.emit('data', len, data);
        }.bind(this));
    };

    close() {
        if (this.uartInstance === null) {
            throw new Error("uart not init");
        }
        __native.UART.close(this.uartInstance);
    };
}

function open(options) {
    return new HW_UART(options);
}

module.exports = {
    open,
}

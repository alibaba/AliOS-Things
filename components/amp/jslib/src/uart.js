import * as UART from 'UART'
import * as event from 'events'

class HW_UART extends event.EventEmitter{
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
        this.uartInstance = UART.open(this.options.id);
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
        this.uartInstance.write(data);
    }

    read() {
        if (this.uartInstance === null) {
        throw new Error("uart not init");
        }
        return this.uartInstance.read();
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
        this.uartInstance.on(function(len, data){
            this.emit('data', len, data);
        }.bind(this));
    };

    close() {
        if (this.uartInstance === null) {
            throw new Error("uart not init");
        }
        this.uartInstance.close();
    };
}

export function open(options) {
    return new HW_UART(options);
}


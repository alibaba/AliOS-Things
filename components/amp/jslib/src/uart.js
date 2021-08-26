import * as UART from 'UART'
import * as event from 'events'

function byteArrayToArrayBuffer(byteArray) {
    return new Uint8Array(byteArray).buffer
}

class HW_UART extends event.EventEmitter{
    constructor(options) {
        super();

        if (!options || !options.id) {
            throw new Error("options is invalid");
        }
        this.options = {
            id: options.id,
            mode: options.mode 
        };

        this.success = options.success || function(){};
        this.fail = options.fail || function(){};
        this._open();
        if (this.options.mode !== 'poll') {
            this._onData();
        } 
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

        if (Object.prototype.toString.call(data) !== '[object String]') {
            this.uartInstance.write(byteArrayToArrayBuffer(data));
        } else {
            this.uartInstance.write(data);
        }
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

        this.uartInstance.on(function(data, len){
            this.emit('data', data, len);
        }.bind(this));
    };

    close() {
        if (this.uartInstance === null) {
            throw new Error("uart not init");
        }
        this.uartInstance.close();
    };

    on_mode() {
        if (this.uartInstance === null) {
            throw new Error("uart not init");
        }
        this._onData();
    };
}

export function open(options) {
    return new HW_UART(options);
}

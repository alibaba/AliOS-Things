'use strict';

const EventEmitter = require('events');

class HW_CAN extends EventEmitter{
    constructor(options) {
        super();

        if (!options || !options.id) {
            throw new Error('options is invalid');
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
        this.canInstance = __native.CAN.open(this.options.id);
        if (!this.canInstance) {
            this.fail();
            return;
        }
        this.success();
    }

    receive() {
        if (!this.canInstance) {
            throw new Error('can not init');
        }
        return __native.CAN.receive(this.canInstance);
    };

    _onData() {
        if (!this.canInstance) {
            throw new Error("can not init");
        }
        __native.CAN.receive(this.canInstance, function(type, id, data){
            this.emit('data', type, id, data);
        }.bind(this));
    };

    send(txheader, data) {
        if (!this.canInstance || !data) {
            throw new Error('can not init or params is invalid');
        }
        __native.CAN.send(this.canInstance, txheader, data);
    };

    close() {
        if (!this.canInstance) {
            throw new Error('can not init');
        }
        __native.CAN.close(this.canInstance);
    };
}

function open(options) {
    return new HW_CAN(options);
}

module.exports = {
    open,
}
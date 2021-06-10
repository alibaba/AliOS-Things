'use strict';

const EventEmitter = require('events');

module.exports = new class Keypad extends EventEmitter {
    constructor(){
        super();
        this._on();
    }

    open() {
        return __native.Keypad.open();
    };

    close() {
        this.removeAllListeners('keypadEvent');
        return __native.Keypad.close();
    };

    _on() {
        __native.Keypad.on(function(code, value) {
            this.emit('keypadEvent', code, value);
        }.bind(this));
    };

}
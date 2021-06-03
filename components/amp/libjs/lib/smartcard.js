'use strict';

const EventEmitter = require('events');

module.exports = new class Smartcard extends EventEmitter {
    constructor(){
        super();
    }

    init() {
        return __native.smartcard.init();
    };

    deinit() {
        return __native.smartcard.deinit();
    };

    select(operator) {
        return __native.smartcard.select(operator);
    };
}
'use strict';

const EventEmitter = require('events');

module.exports = new class UTILS_Battery extends EventEmitter {
    constructor(){
        super();
    }

    getConnectState() {
        return __native.Battery.getConnectState();
    }

    getVoltage() {
        return __native.Battery.getVoltage();
    }

    getLevel() {
        return __native.Battery.getLevel();
    }

    getTemperature() {
        return __native.Battery.getTemperature();
    }
}
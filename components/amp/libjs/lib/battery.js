'use strict';

const EventEmitter = require('events');

module.exports = new class UTILS_Battery extends EventEmitter {
    constructor(){
        super();
    }

    getConnectState() {
        return __native.Battery.get_connect_state();
    }

    getVoltage() {
        return __native.Battery.get_voltage();
    }

    getLevel() {
        return __native.Battery.get_level();
    }

    getTemperature() {
        return __native.Battery.get_temperature();
    }
}
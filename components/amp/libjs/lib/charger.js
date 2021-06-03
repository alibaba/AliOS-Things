'use strict';

const EventEmitter = require('events');

module.exports = new class UTILS_Charger extends EventEmitter {
    constructor(){
        super();
        this._onConnect();
    }

    _onConnect() {
        __native.Charger.onConnect(function(state) {
            this.emit('connect', state);
        }.bind(this));
    };

    getState() {
        return __native.Charger.getState();
    }

    getConnectState() {
        return __native.Charger.getConnectState();
    }

    getCurrent() {
        return __native.Charger.getCurrent();
    }

    switch(onoff) {
        return __native.Charger.switch(onoff);
    }
}
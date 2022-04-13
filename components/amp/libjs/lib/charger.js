'use strict';

const EventEmitter = require('events');

module.exports = new class UTILS_Charger extends EventEmitter {
    constructor(){
        super();
        this._onConnect();
    }

    _onConnect() {
        __native.Charger.on_connect(function(state) {
            this.emit('connect', state);
        }.bind(this));
    };

    getConnectState() {
        return __native.Charger.get_connect_state();
    }

    getCurrent() {
        return __native.Charger.get_current();
    }

    switch(onoff) {
        return __native.Charger.switch(onoff);
    }
}
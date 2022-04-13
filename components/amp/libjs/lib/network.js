'use strict';

const EventEmitter = require('events');

class netWork extends EventEmitter{
    constructor() {
        super();

        this._connect();
    }

    _connect() {
        var cb = function(status) {
            if (status === 1) {
                this.emit('connect');
            } else {
                this.emit('disconnect');
            } 
        }
        if(__native.CELLULAR.onConnect(cb.bind(this)) !== 0){
            this.emit('error', 'network status error');
        }
    }

    getType() {
        var type;
        var imei = __native.CELLULAR.getInfo().imei;
        if (imei === null){
            type = 'wifi';
            return type;
        }
        type = 'cellular';
        return type;
    }

    connect(options) {
        var type = this.getType();
        if (type != 'wifi') {
            console.log('net invalid');
            return;
        }
        __native.WIFI.connect(options.ssid, options.password, function(status) {
            if (status == 'DISCONNECT'){
                this.emit('disconnect', options.ssid);
                return;
            }
            this.emit('connect', options.ssid);
        }.bind(this));
    }

    disconnect() {
        var type = this.getType();
        if (type != 'wifi') {
            console.log('net invalid');
            return;
        }
        var ssid = __native.WIFI.getInfo().ssid;
        var ip = __native.WIFI.getInfo().ip;
        if (ip == null) {
            console.log('no net connected');
            return;
        }
        __native.WIFI.disconnect();
        this.emit('disconnect', ssid);
    }

    getInfo() {
        var info = {};
        var type = this.getType();
        if (type == 'wifi') {
            info = __native.WIFI.getInfo();
            return info;
        }

        if (type == 'cellular') {
            var info = __native.CELLULAR.getInfo();
            return info;

        }
        console.log('net invalid');
        return;
    }

    getStatus() {
        var type = this.getType();
        if (type == 'wifi') {
            var ip = __native.WIFI.getInfo().ip
            if (ip == '0.0.0.0') {
                console.log('ip null')
                return 'disconnect';
            }
            return 'connect'
        }
        if (type == 'cellular') {
            if (__native.CELLULAR.getStatus() != 1) {
                return 'disconnect';
            }
            return 'connect';
        }
    }
}

function openNetWorkClient() {
    return new netWork();
}

module.exports = {
    openNetWorkClient,
}
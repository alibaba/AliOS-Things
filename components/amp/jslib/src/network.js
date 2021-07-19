'use strict';
import * as events from 'events'
import * as NETWORK from 'NETWORK'
import * as CELLULAR from 'CELLULAR'

class netWork extends events.EventEmitter{
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
        var type = this.getType();
        if (type == 'wifi') {
            return;
        }
        if(CELLULAR.onConnect(cb.bind(this)) !== 0){
            this.emit('error', 'network status error');
        }
    }

    getType() {
        var type = NETWORK.getType();
        switch (type) {
            case 0:
                type = 'wifi'; break;
            case 1:
                type = 'cellular'; break;
            case 2:
                type = 'ethnet'; break;
            default:
                type = 'unknow'; break;
        }

        return type;
    }

    connect(options) {
        var type = this.getType();
        if (type != 'wifi') {
            console.log('net invalid');
            return;
        }        
    }

    disconnect() {
        var type = this.getType();
        if (type != 'wifi') {
            console.log('net invalid');
            return;
        }        
    }

    getInfo() {
        var info = {
            simInfo: null,
            locatorInfo: null,
            wifiInfo: null
        };
        var type = this.getType();
        if (type == 'wifi') {            
            return info;
        }

        if (type == 'cellular') {
            info.simInfo = CELLULAR.getSimInfo();
            info.locatorInfo = CELLULAR.getLocatorInfo();
            return info;
        }
        console.log('net invalid');
        return;
    }

    getStatus() {
        var type = this.getType();
        if (type == 'wifi') {           
            return 'connect';
        }
        if (type == 'cellular') {
            if (CELLULAR.getStatus() != 1) {
                return 'disconnect';
            }
            return 'connect';
        }
    }
}

export function openNetWorkClient() {
    return new netWork();
}

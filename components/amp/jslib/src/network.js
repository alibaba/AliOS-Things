'use strict';
import * as std from 'std'
import * as events from 'events'
import * as NETWORK from 'NETWORK'

class netWork extends events.EventEmitter {
    constructor() {
        super();

        var devType = this.getType();
        console.log('devType is ' + devType);
        if (devType == 'wifi') {
            std.eval('import * as NETMGR from \'NETMGR\'; globalThis.NETMGR = NETMGR')
            this.name = '/dev/wifi0';
            this._wifiInit();
            this.dev_handler = this._wifiGetDev();
        }
        else if (devType == 'cellular') {
            std.eval('import * as CELLULAR from \'CELLULAR\'; globalThis.CELLULAR = CELLULAR')
            this._connect();
        }
    }

    _wifiInit() {
        if (NETMGR.serviceInit() !== 0) {
            this.emit('error', 'netmgr init error');
        }
    }

    _wifiGetDev() {
        console.log('wifi name:' + this.name)
        var dev_handler = NETMGR.getDev(this.name);
        if (dev_handler == -1) {
            console.log('netmgr get wifi dev error: ' + this.name);
            return;
        }

        return dev_handler;
    }

    _connect() {
        var cb = function (status) {
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
        if (CELLULAR.onConnect(cb.bind(this)) !== 0) {
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
            console.log('Not wifi dev, can\'t connect');
            return;
        }

        options = {
            ssid: options.ssid,
            password: options.password,
            bssid: options.bssid || '',
            timeout_ms: options.timeout_ms || 18000
        }

        NETMGR.connect(this.dev_handler, options, function (status) {
            if (status == 'DISCONNECT') {
                this.emit('disconnect', options.ssid);
                return;
            }
            this.emit('connect', options.ssid);
        }.bind(this));
    }

    disconnect() {
        var type = this.getType();
        if (type != 'wifi') {
            console.log('Not wifi dev, can\'t disconnect');
            return;
        }

        var ret = NETMGR.disconnect(this.dev_handler);
        if (ret !== 0) {
            this.emit('error', 'netmgr disconnect error');
            return;
        }
        this.emit('disconnect', ssid);
    }

    getInfo() {
        var info = {
            simInfo: null,
            locatorInfo: null,
            wifiInfo: null
        };
        var type = this.getType();
        if (type == 'wifi') {
            info.wifiInfo = this.wifiGetIfConfig();
            return info;
        }

        if (type == 'cellular') {
            info.simInfo = CELLULAR.getSimInfo();
            info.locatorInfo = CELLULAR.getLocatorInfo();
            return info;
        }
        console.log('net mode invalid');
        return;
    }

    getStatus() {
        var type = this.getType();
        if (type == 'wifi') {
            var ret = NETMGR.getState(this.dev_handler);
            if (ret == 5) {
                return 'connect';
            } else {
                return 'disconnect';
            }
        }
        if (type == 'cellular') {
            if (CELLULAR.getStatus() != 1) {
                return 'disconnect';
            }
            return 'connect';
        }
    }

    wifiSaveConfig() {
        var ret = NETMGR.saveConfig(this.dev_handler);
        if (ret !== 0) {
            this.emit('error', 'netmgr save config error');
        }
    }

    wifiSetIfConfig(options) {

        options = {
            dhcp_en: options.dhcp_en || true,
            ip_addr: options.ip_addr || '',
            mask: options.mask || '',
            gw: options.gw || '',
            dns_server: options.dns_server || '',
            mac: options.mac || ''
        }

        var ret = NETMGR.setIfConfig(this.dev_handler, options);
        if (ret !== 0) {
            this.emit('error', 'netmgr save config error');
        }
    }

    wifiGetIfConfig() {
        var config = NETMGR.getIfConfig(this.dev_handler);
        if (!config) {
            this.emit('error', 'get if config error');
        }
        return config;
    }
}

export function openNetWorkClient() {
    return new netWork();
}

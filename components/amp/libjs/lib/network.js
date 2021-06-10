'use strict';

const EventEmitter = require('events');

class netWork extends EventEmitter{
    constructor(options) {
        super();

        if(!options || !options.type){
            throw new Error('net type is error');
        }

        this.options = {
            type: options.type || 'cellular',
            devName: options.devName || ''
        };

        this._init();
        this._cellularStatus();
        this.dev_handler = this._getDev(this.devName);
    }

    _init() {
        if (__native.NETMGR.serviceInit() !== 0) {
            this.emit('error', 'netmgr init error');
        }
    }

    _getDev(devName) {
        var dev_handler = __native.NETMGR.getDev(devName);
        if (!dev_handler){
            this.emit('error', 'netmgr get dev error ' + devName);
            return;
        }

        return dev_handler;
    }

    _cellularStatus() {
        var cb = function(status) {
            if (status === 1) {
                this.emit('connect');
            } else {
                this.emit('disconnect');
            }
        }
        var type = this.type;
        if (type != 'cellular') {
            return;
        }
        if(__native.CELLULAR.onConnect(cb.bind(this)) !== 0){
            this.emit('error', 'network status error');
        }
    }

    setMsgCb(cb) {
        __native.NETMGR.setMsgCb(this.dev_handler, cb);
    }

    delMsgCb(cb) {
        __native.NETMGR.delMsgCb(this.dev_handler, cb);
    }

    setAutoReconnect(flag) {
        var ret = __native.NETMGR.setAutoReconnect(this.dev_handler, flag);
        if (ret !== 0) {
            this.emit('error', 'netmgr set auto reconnect error');
        }
    }

    connect(options) {
        options = {
            ssid: options.ssid,
            password: options.password,
            bssid: options.bssid || '',
            timeout_ms: options.timeout_ms || 18000
        }

        __native.NETMGR.connect(this.dev_handler, options, function(status) {
            if (status == 'DISCONNECT'){
                this.emit('disconnect', options.ssid);
                return;
            }
            this.emit('connect', options.ssid);
        }.bind(this));
    }

    disconnect() {
        var ret = __native.NETMGR.disconnect(this.dev_handler);
        if (ret !== 0) {
            this.emit('error', 'netmgr disconnect error');
            return;
        }
        this.emit('disconnect', ssid);
    }

    getStatus() {
        if (this.type == 'wifi') {
            var ret = __native.NETMGR.getState(this.dev_handler);
            switch (ret) {
            case 0:
                return 'disconnecting';
            case 1:
                return 'disconnected';
            case 2:
                return 'connecting';
            case 3:
                return 'connected';
            case 4:
                return 'obtaining ip';
            case 5:
                return 'network connected';
            case 6:
                return 'failed';
            }
        }
        if (this.type == 'cellular') {
            if (__native.CELLULAR.getStatus() != 1) {
                return 'disconnect';
            }
            return 'connect';
        }
    }

    getInfo() {
        var info = {
            simInfo: null,
            locatorInfo: null,
            wifiInfo: null
        };
        if (this.type == 'wifi') {
            info.wifiInfo = __native.WIFI.getIfConfig();
            return info;
        }

        if (this.type == 'cellular') {
            info.simInfo = __native.CELLULAR.getSimInfo();
            info.locatorInfo = __native.CELLULAR.getLocatorInfo();
            return info;
        }

        return;
    }

    saveConfig() {
        var ret = __native.NETMGR.saveConfig(this.dev_handler);
        if (ret !== 0) {
            this.emit('error', 'netmgr save config error');
        }
    }

    setIfConfig(options) {
        options = {
            dhcp_en: options.dhcp_en || true,
            ip_addr: options.ip_addr || '',
            mask: options.mask || '',
            gw: options.gw || '',
            dns_server: options.dns_server || '',
            mac: options.mac || ''
        }
        var ret = __native.NETMGR.setIfConfig(this.dev_handler, options);
        if (ret !== 0) {
            this.emit('error', 'netmgr save config error');
        }
    }

    getIfConfig() {
        var config = __native.NETMGR.getIfConfig(this.dev_handler);
        if (!config) {
            this.emit('error', 'get if config error');
        }
    }
}

function openNetWorkClient(options) {
    return new netWork(options);
}

module.exports = {
    openNetWorkClient,
}
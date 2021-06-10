'use strict';

import * as NETMGR from 'NETMGR'
import * as events from 'events'

class netMgr extends events.EventEmitter{
    constructor(options) {
        super();

        if(!options || !options.name){
            throw new Error('device info error');
        }

        this.options = {
            name: options.name
        };

        this.name = options.name
        this._init();
        this.dev_handler = this._getDev();
    }

    _init() {
        if(NETMGR.serviceInit() !== 0){
            this.emit('error', 'netmgr init error');
        }
    }

    _getDev() {
        console.log('wifi name:' + this.name)
        var dev_handler = NETMGR.getDev(this.name);
        if (!dev_handler){
            this.emit('error', 'netmgr get dev error ' + this.name);
            return;
        }

        return dev_handler;
    }

    setMsgCb(cb) {
        NETMGR.setMsgCb(this.dev_handler, cb);
    }

    delMsgCb(cb) {
        NETMGR.delMsgCb(this.dev_handler, cb);
    }

    setAutoReconnect(flag) {
        var ret = NETMGR.setAutoReconnect(this.dev_handler, flag);
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

        NETMGR.connect(this.dev_handler, options, function(status) {
            if (status == 'DISCONNECT'){
                this.emit('disconnect', options.ssid);
                return;
            }
            this.emit('connect', options.ssid);
        }.bind(this));
    }

    disconnect() {
        var ret = NETMGR.disconnect(this.dev_handler);
        if (ret !== 0) {
            this.emit('error', 'netmgr disconnect error');
            return;
        }
        this.emit('disconnect', ssid);
    }

    getState() {
        var ret = NETMGR.getState(this.dev_handler);
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

    saveConfig() {
        var ret = NETMGR.saveConfig(this.dev_handler);
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

        var ret = NETMGR.setIfConfig(this.dev_handler, options);
        if (ret !== 0) {
            this.emit('error', 'netmgr save config error');
        }
    }

    getIfConfig() {
        var config = NETMGR.getIfConfig(this.dev_handler);
        if (!config) {
            this.emit('error', 'get if config error');
        }
        return config;
    }
}

export function openNetMgrClient(options) {
    return new netMgr(options);
}

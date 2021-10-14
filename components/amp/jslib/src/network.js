'use strict';
import * as std from 'std'
import * as events from 'events'
import * as NETWORK from 'NETWORK'

var devType = 0;
class netWork extends events.EventEmitter {
    constructor(options) {
        super();

        if (!options.devType) {
            devType = this.getType();
        }
        else {
            devType = options.devType;
        }

        if(devType != 'wifi' && devType != 'cellular' && devType != 'ethnet') {
            throw new Error('devType only support cellular/wifi/ethnet, please check input!');
            return;
        }

        console.log('devType is ' + devType);
        if (devType == 'wifi') {
            std.eval('import * as NETMGR from \'NETMGR\'; globalThis.NETMGR = NETMGR')
            this.name = '/dev/wifi0';
            this._netmgrInit();
            this.dev_handler = this._netmgrGetDev();
        }
        else if (devType == 'ethnet') {
            std.eval('import * as NETMGR from \'NETMGR\'; globalThis.NETMGR = NETMGR')
            this.name = '/dev/eth0';
            this._netmgrInit();
            this.dev_handler = this._netmgrGetDev();

            // this._onConnect();
        }
        else if (devType == 'cellular') {
            std.eval('import * as CELLULAR from \'CELLULAR\'; globalThis.CELLULAR = CELLULAR')
            this._onConnect();
        }
    }

    _netmgrInit() {
        if (NETMGR.serviceInit() !== 0) {
            this.emit('error', 'netmgr init error');
        }
    }

    _netmgrGetDev() {
        console.log('device net name:' + this.name)
        var dev_handler = NETMGR.getDev(this.name);
        if (dev_handler == -1) {
            console.log('netmgr get dev handle error: ' + this.name);
            return;
        }

        return dev_handler;
    }

    _onConnect() {
        if (devType == 'cellular') {
            var cb = function (status) {
                if (status === 1) {
                    this.emit('connect');
                } else {
                    this.emit('disconnect');
                }
            }

            if (CELLULAR.onConnect(cb.bind(this)) !== 0) {
                this.emit('error', 'network on cellular status error');
            }
            return;
        }

        if (devType == 'ethnet') {
            var cb_ = function(status) {
                if (status == 'DISCONNECT') {
                    console.log('ethnet  disconnect');
                    this.emit('disconnect');
                } else {
                    console.log('ethnet  connect');
                    this.emit('connect');
                }
            }

            if (NETMGR.connect(this.dev_handler, cb_.bind(this)) !== 0) {
                this.emit('error', 'network on ethnet status error');
            }
            return;
        }
    }

    /**
     * get device net type
    */
    getType() {
        if (devType == 0) {
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
        } else {
            return devType;
        }
    }

    /**
     * only wifi device support connect
    */
    connect(options) {
        if (devType != 'wifi') {
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

    /**
     * only wifi device support disconnect
    */
    disconnect() {
        if (devType != 'wifi') {
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
            netInfo: null
        };
        var type = devType;
        if (type == 'wifi' || type == 'ethnet') {
            info.netInfo = this.getIfConfig();
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
        var type = devType;
        if (type == 'wifi' || type == 'ethnet') {
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

    saveConfig() {
        if (devType == 'cellular') {
            throw new Error('cellular device isn\'t support saveConfig!');
            return;
        }

        var ret = NETMGR.saveConfig(this.dev_handler);
        if (ret !== 0) {
            throw new Error('netmgr save config error');
        }
    }

    setIfConfig(options) {
        if (devType == 'cellular') {
            throw new Error('setIfConfig: cellular device isn\'t support!');
            return;
        }

        if (!options.hasOwnProperty('dhcp_en')) {
            throw new Error('setIfConfig: property dhcp_en undefined and should be bool type!');
            return;
        }

        options = {
            dhcp_en: options.dhcp_en,
            ip_addr: options.ip_addr || '',
            mask: options.mask || '',
            gw: options.gw || '',
            dns_server: options.dns_server || ''
        }

        if (options.dhcp_en) {
            console.log('setIfConfig: enable dhcp');
        }

        var ret = NETMGR.setIfConfig(this.dev_handler, options);
        if (ret !== 0) {
            throw new Error('netmgr save config error');
        }

        this._onConnect();
    }

    getIfConfig() {
        if (devType == 'cellular') {
            throw new Error('cellular device isn\'t support getIfConfig!');
            return;
        }

        var config = NETMGR.getIfConfig(this.dev_handler);
        if (!config) {
            throw new Error('get if config error');
        }
        return config;
    }
}

export function openNetWorkClient(options) {
    return new netWork(options);
}

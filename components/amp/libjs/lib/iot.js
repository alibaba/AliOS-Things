'use strict';

const EventEmitter = require('events');

class IotDeviceClient extends EventEmitter {
    constructor(options){
        super();
        if(!options || !options.productKey || !options.deviceName || !options.deviceSecret){
            throw new Error('device info error');
        }

        this.options = {
            productKey: options.productKey,
            deviceName: options.deviceName,
            deviceSecret: options.deviceSecret,
            region: options.region || 'cn-shanghai',
            keepaliveSec: options.keepaliveSec || 60
        }

        this._connect();
    }

    _connect() {
        __native.AIOT_DEVICE.device(this.options, function(res) {
            if (!res.handle) {
                return;
            }
            this.IoTDeviceInstance = res.handle;
            switch (res.code) {
                case 0:
                    this.emit('connect'); break;
                case 1:
                    this.emit('reconnect'); break;
                case 2:
                    this.emit('disconnect'); break;
                case 3:
                    this.emit('message', res); break;
                default : break ;
            }
        }.bind(this));
    }

    getDeviceHandle() {
        return this.IoTDeviceInstance;
    }

    subscribe(options, cb) {
        var ret = __native.AIOT_DEVICE.subscribe(this.IoTDeviceInstance, options, cb || function() {});
        if (ret < 0) {
            throw new Error('subscribe topic error', options.topic);
        }

        return ret;
    }

    unsubscribe(topic, cb) {
        var ret = __native.AIOT_DEVICE.unsubscribe(this.IoTDeviceInstance, topic, cb || function() {});
        if (ret < 0) {
            throw new Error('unsubscribe topic error', topic);
        }

        return ret;
    }

    publish(options, cb) {
        var ret = __native.AIOT_DEVICE.publish(this.IoTDeviceInstance, options, cb || function() {});
        if (ret < 0) {
            throw new Error('publish topic info error', options.topic);
        }

        return ret;
    }

    postProps(params, cb) {
        var ret = __native.AIOT_DEVICE.postProps(this.IoTDeviceInstance, params, cb || function() {});
        if (ret < 0) {
            throw new Error('post props error');
        }

        return ret;
    }

    onProps(cb) {
        var ret = __native.AIOT_DEVICE.onProps(this.IoTDeviceInstance, cb);
        if (ret < 0) {
            throw new Error('on props error');
        }

        return ret;
    }

    postEvent(options, cb) {
        var ret = __native.AIOT_DEVICE.postEvent(this.IoTDeviceInstance, options, cb || function() {});
        if (ret < 0) {
            throw new Error('post event error');
        }

        return ret;
    }

    onService(cb) {
        var ret = __native.AIOT_DEVICE.onService(this.IoTDeviceInstance, cb);
        if (ret < 0) {
            throw new Error('on service error');
        }

        return ret;
    }

    end(cb) {
        var ret = __native.AIOT_DEVICE.close(this.IoTDeviceInstance, cb || function() {});
        if (ret < 0) {
            throw new Error('end iot client error');
        }

        return ret;
    }
}

class IotGatewayClient extends EventEmitter{
    constructor(options){
        super();
        if(!options || !options.productKey || !options.deviceName || !options.deviceSecret){
            throw new Error('device info error');
        }

        this.options = {
            productKey: options.productKey,
            deviceName: options.deviceName,
            deviceSecret: options.deviceSecret,
            keepaliveSec: options.keepaliveSec || 60,
            region: options.region || 'cn-shanghai'
        }

        this._on();
        this._connect();
    }

    _connect() {
        __native.AIOT_GATEWAY.gateway(this.options, function(res) {
            if (!res.handle || res.code != 0) {
                return;
            }
            this.IoTGatewayInstance = res.handle;
            this.emit('connect');
        }.bind(this));
    }

    _on() {
        __native.AIOT_GATEWAY.onMqttMessage(function(res) {
            switch (res.code) {
                case 1:
                    this.emit('reconnect'); break;
                case 2:
                    this.emit('disconnect'); break;
                case 3:
                    this.emit('message', res); break;
                default : break ;
            }
        }.bind(this));
    }

    addTopo(options, cb) {
        var ret = __native.AIOT_GATEWAY.addTopo(this.IoTGatewayInstance, options, cb || function(ret, message) {});
        if (ret < 0) {
            throw new Error('add topo error');
        }

        return ret;
    }

    getTopo(cb) {
        var ret = __native.AIOT_GATEWAY.getTopo(this.IoTGatewayInstance, cb || function(ret, message) {});
        if (ret < 0) {
            throw new Error('get topo error');
        }

        return ret;
    }

    removeTopo(options, cb) {
        var ret = __native.AIOT_GATEWAY.removeTopo(this.IoTGatewayInstance, options, cb || function() {});
        if (ret < 0) {
            throw new Error('remove topo error');
        }

        return ret;
    }

    login(options, cb) {
        var ret = __native.AIOT_GATEWAY.login(this.IoTGatewayInstance, options, cb || function() {});
        if (ret < 0) {
            throw new Error('aiot subdev login error');
        }

        return ret;
    }

    logout(options, cb) {
        var ret = __native.AIOT_GATEWAY.logout(this.IoTGatewayInstance, options, cb || function() {});
        if (ret < 0) {
            throw new Error('aiot subdev logout error');
        }

        return ret;
    }

    registerSubDevice(options, cb) {
        var ret = __native.AIOT_GATEWAY.registerSubDevice(this.IoTGatewayInstance, options, cb || function(ret, message) {});
        if (ret < 0) {
            throw new Error('aiot register subdev error');
        }

        return ret;
    }

    subscribe(params, cb) {
        var ret = __native.AIOT_GATEWAY.subscribe(this.IoTGatewayInstance, params, cb || function() {});
        if (ret < 0) {
            throw new Error('subscribe topic error', options.topic);
        }

        return ret;
    }

    unsubscribe(topic, cb) {
        var ret = __native.AIOT_GATEWAY.unsubscribe(this.IoTGatewayInstance, topic, cb || function() {});
        if (ret < 0) {
            throw new Error('unsubscribe topic error', topic);
        }

        return ret;
    }

    publish(options, cb) {
        var ret = __native.AIOT_GATEWAY.publish(this.IoTGatewayInstance, options, cb || function() {});
        if (ret < 0) {
            throw new Error('publish topic info error', options.topic);
        }

        return ret;
    }
}

function dynreg(options, cb) {
    var ret = __native.AIOT_DEVICE.register(options, cb);
        if (ret < 0) {
            throw new Error('dynmic register error');
        }

    return ret;
}

function device(options) {
    return new IotDeviceClient(options);
}

function gateway(options){
    return new IotGatewayClient(options);
}

module.exports = {
    device,
    gateway,
    dynreg
}
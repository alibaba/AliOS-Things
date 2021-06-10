import * as event from 'events'
import * as GATEWAY from 'AIOT_GATEWAY'

class IotGatewayClient extends event.EventEmitter{
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
        GATEWAY.gateway(this.options, function(res) {
            if (!res.handle || res.code != 0) {
                return;
            }
            this.IoTGatewayInstance = res.handle;
            this.emit('connect');
        }.bind(this));
    }

    _on() {
        GATEWAY.onMqttMessage(function(res) {
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
        var ret = this.IoTGatewayInstance.addTopo(options, cb || function(ret, message) {});
        if (ret < 0) {
            throw new Error('add topo error');
        }

        return ret;
    }

    getTopo(cb) {
        var ret = this.IoTGatewayInstance.getTopo(cb || function(ret, message) {});
        if (ret < 0) {
            throw new Error('get topo error');
        }

        return ret;
    }

    removeTopo(options, cb) {
        var ret = this.IoTGatewayInstance.removeTopo(options, cb || function() {});
        if (ret < 0) {
            throw new Error('remove topo error');
        }

        return ret;
    }

    login(options, cb) {
        var ret = this.IoTGatewayInstance.login(options, cb || function() {});
        if (ret < 0) {
            throw new Error('aiot subdev login error');
        }

        return ret;
    }

    logout(options, cb) {
        var ret = this.IoTGatewayInstance.logout(options, cb || function() {});
        if (ret < 0) {
            throw new Error('aiot subdev logout error');
        }

        return ret;
    }

    registerSubDevice(options, cb) {
        var ret = this.IoTGatewayInstance.registerSubDevice(options, cb || function(ret, message) {});
        if (ret < 0) {
            throw new Error('aiot register subdev error');
        }

        return ret;
    }

    subscribe(params, cb) {
        var ret = this.IoTGatewayInstance.subscribe(params, cb || function() {});
        if (ret < 0) {
            throw new Error('subscribe topic error', options.topic);
        }

        return ret;
    }

    unsubscribe(topic, cb) {
        var ret = this.IoTGatewayInstance.unsubscribe(topic, cb || function() {});
        if (ret < 0) {
            throw new Error('unsubscribe topic error', topic);
        }

        return ret;
    }

    publish(options, cb) {
        var ret = this.IoTGatewayInstance.publish(options, cb || function() {});
        if (ret < 0) {
            throw new Error('publish topic info error', options.topic);
        }

        return ret;
    }
}

export function gateway(options){
    console.log("create IotGatewayClient");
    return new IotGatewayClient(options);
}

import * as event from 'events'
import * as AIOT_DEVICE from 'AIOT_DEVICE'
import * as AIOT_GATEWAY from 'AIOT_GATEWAY'

class IotDeviceClient extends event.EventEmitter {
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
            keepaliveSec: 60
        }

        this._connect();
    }

    _connect() {
        this.IoTDeviceInstance = AIOT_DEVICE.device(this.options, function(res) {
            if (!res.handle) {
                console.log("res.handle is empty");
                return;
            }
            this.iot_device_handle = res.handle;
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
        return this.iot_device_handle;
    }

    subscribe(options, cb) {
        console.log("subscribe is called");
        var ret = this.IoTDeviceInstance.subscribe(options, cb || function() {});
        if (ret < 0) {
            throw new Error('subscribe topic error', options.topic);
        }

        return ret;
    }

    unsubscribe(topic, cb) {
        var ret = this.IoTDeviceInstance.unsubscribe(topic, cb || function() {});
        if (ret < 0) {
            throw new Error('unsubscribe topic error', topic);
        }

        return ret;
    }

    publish(options, cb) {
        var ret = this.IoTDeviceInstance.publish(options, cb || function() {});
        if (ret < 0) {
            throw new Error('publish topic info error', options.topic);
        }

        return ret;
    }

    getNtpTime(cb) {
        var ret = this.IoTDeviceInstance.getNtpTime(cb || function() {});
        if (ret < 0) {
            throw new Error('get ntp time error');
        }
        return ret;
    }
    postProps(params, cb) {
        console.log("postProps is called");
        var ret = this.IoTDeviceInstance.postProps(params, cb || function() {});
        if (ret < 0) {
            throw new Error('post props error');
        }

        return ret;
    }

    onProps(cb) {
        var ret = this.IoTDeviceInstance.onProps(cb);
        if (ret < 0) {
            throw new Error('on props error');
        }

        return ret;
    }

    postEvent(options, cb) {
        console.log("postEvent is called");
        var ret = this.IoTDeviceInstance.postEvent(options, cb || function() {});
        if (ret < 0) {
            throw new Error('post event error');
        }

        return ret;
    }

    onService(cb) {
        var ret = this.IoTDeviceInstance.onService(cb);
        if (ret < 0) {
            throw new Error('on service error');
        }

        return ret;
    }

    end(cb) {
        var ret = this.IoTDeviceInstance.close(cb || function() {});
        if (ret < 0) {
            throw new Error('end iot client error');
        }

        return ret;
    }
}


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
        this.IoTGatewayInstance = AIOT_GATEWAY.gateway(this.options, function(res) {
            if (!res.handle || res.code != 0) {
                return;
            }
            this.iot_gateway_handle = res.handle;
            this.emit('connect');
        }.bind(this));
    }

    getGatewayHandle() {
        return this.iot_gateway_handle;
    }

    _on() {
        AIOT_GATEWAY.onMqttMessage(function(res) {
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

export function device(options) {
    console.log("create IotDeviceClient");
    return new IotDeviceClient(options);
}

export function gateway(options){
    console.log("create IotGatewayClient");
    return new IotGatewayClient(options);
}

export function dynreg(options, cb) {
    var ret = AIOT_DEVICE.register(options, cb);
    if (ret < 0) {
        throw new Error('dynmic register error');
    }

    return ret;
}
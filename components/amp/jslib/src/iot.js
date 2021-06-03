import * as event from 'events'
import * as IOT from 'AIOT_DEVICE'

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
        IOT.device(this.options, function(res) {
            if (!res.handle) {
                console.log("res.handle is empty");
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

export function device(options) {
    console.log("create IotDeviceClient");
    return new IotDeviceClient(options);
}

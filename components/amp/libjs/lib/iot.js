'use strict';

const EventEmitter = require('events');

class IotClient extends EventEmitter{
    constructor(options){
        super();
        if(!options || !options.productKey || !options.deviceName || !options.deviceSecret){
            throw new Error('device info error');
        }

        this.options = {
            productKey: options.productKey,
            deviceName: options.deviceName,
            deviceSecret: options.deviceSecret,
            region: options.region || 'cn-shanghai'
        }

        this.fail = options.fail || function(){};
        this.success = options.success || function(){};
        this.connected = false;
        this._connect();
        this._on();
    }

    _connect() {
        this.IoTInstance = __native.IOT.device(this.options, function(ret) {
            if (ret < 0) {
                this.connected = false;
                this.fail();
                this.emit('error', ret);
                return;
            }
            this.connected = true;
            this.success();

            __native.IOT.onProps(this.IoTInstance, function(req_len, req) {
                this.emit('props', JSON.parse(req));
            }.bind(this));

            __native.IOT.onService(this.IoTInstance, function(serviceid, req) {
                this.emit('service', serviceid, req);
            }.bind(this));
        }.bind(this));
    }

    postProps(options) {
        if(this.IoTInstance < 0 || !options || !options.payload){
            throw new Error('device not init or invalid params');
        }
        var ret = __native.IOT.postProps(this.IoTInstance, JSON.stringify(options.payload));
        if (ret < 0) {
            if(typeof options.fail === 'function') {
                options.fail();
            }
            this.emit('error', options.payload);
            return;
        }

        if(typeof options.success === 'function') {
            options.success();
        }
    }

    _on() {
        if(this.IoTInstance < 0){
            throw new Error('device not init...');
        }

        __native.IOT.onConnect(this.IoTInstance, function() {
            this.emit('connect');
        }.bind(this));

        __native.IOT.onDisconnect(this.IoTInstance, function() {
            this.emit('disconnect');
        }.bind(this));
    }

    postEvent(options) {
        if(this.IoTInstance < 0 || !options || !options.id || !options.params){
            throw new Error('device not init or invalid params');
        }
        options.params = JSON.stringify(options.params);
        var ret = __native.IOT.postEvent(this.IoTInstance, options);
        if (ret < 0) {
            if(typeof options.fail === 'function') {
                options.fail();
            }
            this.emit('error', 'postEvent id: ' + options.id);
            return;
        }
        if(typeof options.success === 'function') {
            options.success();
        }
    }

    close() {
        if(this.IoTInstance < 0){
            throw new Error('device not init...');
        }
        var ret = __native.IOT.close(this.IoTInstance);
        if (ret != 0) {
            this.emit('error', 'iot client close falied')
            return
        }
        this.emit('close');
    }
}

function device(options){
    return new IotClient(options);
}

function sign(options) {
    if(!options || !options.productKey || !options.deviceName || !options.deviceSecret){
        throw new Error('sign info error');
    }

    this.options = {
        productKey: options.productKey,
        deviceName: options.deviceName,
        deviceSecret: options.deviceSecret,
        region: options.region || 'cn-shanghai'
    }

    this._fail = options.fail || function() {};
    this._success = options.success || function() {};

    var result = __native.IOT.sign(this.options, function(ret, mqttParam) {
        if(ret < 0) {
            /* sign failed */
            this._fail();
            return;
        }

        this._success(mqttParam);
    }.bind(this));

    if(result < 0) {
        this._fail();
        return;
    }
}
module.exports = {
    device,
    sign,
}
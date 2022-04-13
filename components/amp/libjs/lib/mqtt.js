'use strict';

const EventEmitter = require('events');

class MQTTClient extends EventEmitter{
    constructor(options) {
        super();
        if (!options || !options.host) {
            throw new Error('options is invalid');
        }
        
        this.options = {
            host: options.host,
            port: options.port || 1883,
            client_id: options.clientId || this._getRandomClientId(),
            username: options.username || '',
            password: options.password || '',
            keepalive_interval: options.keepalive_interval || 60
        };
    
        this._fail = options.fail || function(){};
        this._success = options.success || function(){};
        this.connected = false;
        this._connect();
    }

    _getRandomClientId() {
        return 'amp-' + parseInt(Math.random() * 1000000);
    }

    _connect() {
        var cb = function(err){
            if(err === -2) {
                // connect failed
                this._fail();
                this.emit('error', 'connect failed');
            }

            if(err === -1) {
                // status: disconnect
                this.connected = false;
                this.emit('disconnect');
            }
            if(err === 0) {
                // status: connect
                this.connected = true;
                this._success();
                this.emit('connect');
            }
        };
        this.mqttInstance = __native.MQTT.start(this.options, cb.bind(this));
        if (!this.mqttInstance){
            // connect failed
            this._fail();
            this.emit('error', 'connect failed');
            return
        }
    }

    subscribe(options) {
        if (!this.mqttInstance || !options || !options.topic) {
            throw new Error('mqtt not init or options invalid');
        }

        if(this.connected === false) {
            this.emit('error', 'subscirbe fail: not connected');
            return;
        }

        var ret = __native.MQTT.subscribe(this.mqttInstance, options.topic, options.qos || 0, function(topic, payload){
            this.emit('message', topic, payload);
        }.bind(this));
        if (ret < 0) {
            if(typeof options.fail === 'function') {
                options.fail();
            }
            this.emit('error', 'subscribe error');
        }
        else {
            if(typeof options.success === 'function') {
                options.success();
            }
        }
    }

    unsubscribe(options) {
        if (!this.mqttInstance || !options || !options.topic) {
            throw new Error('mqtt not init or mqtt topic is invalid');
        }

        if(this.connected === false) {
            this.emit('error', 'unsubscribe fail: not connected');
            return;
        }

        var ret = __native.MQTT.unsubscribe(this.mqttInstance, options.topic, function() {
        }.bind(this));
        if (ret < 0) {
            if(typeof options.fail === 'function') {
                options.fail();
            }
            this.emit('error', 'unsubscribe error');
            return
        }

        if(typeof options.success === 'function') {
            options.success();
        }
    }

    publish(options) {
        if (!this.mqttInstance || !options || !options.topic || !options.message) {
            throw new Error('mqtt not init or options invalid');
        }

        if(this.connected === false) {
            this.emit('error', 'publish fail: not connected');
            return;
        }

        __native.MQTT.publish(this.mqttInstance, options.topic, options.message, options.qos || 0, function(ret) {
        if (ret < 0) {
            if(typeof options.fail === 'function') {
                options.fail();
            }
            this.emit('error', options.topic);
            return;
        }

        if(typeof options.success === 'function') {
            options.success();
        }
        }.bind(this));
    }

    close() {
        if (!this.mqttInstance) {
            throw new Error('mqtt not init');
        }
        __native.MQTT.close(this.mqttInstance, function(ret){
            if (ret != 0) {
                this.emit('error', 'mqtt client close error');
                return;
            }
            this.emit('close');
        }.bind(this));

    }
}

function createClient(options) {
    return new MQTTClient(options);
}

module.exports = {
    createClient,
}
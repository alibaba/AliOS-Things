import * as event from 'events'
import * as MQTT from 'MQTT'

class MQTTClient extends event.EventEmitter {
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

        this._fail = options.fail || function () { };
        this._success = options.success || function () { };
        this.connected = false;
        this.mqttInstance = 0;
        this._connect();
    }

    _getRandomClientId() {
        return 'amp-' + parseInt(Math.random() * 1000000);
    }

    _connect() {
        MQTT.start(this.options, function (res) {
            if (!res.handle) {
                return;
            }
            this.mqttInstance = res.handle;
            switch (res.code) {
                case 0:
                    this.connected = true;
                    this.emit('connect'); break;
                case 1:
                    this.connected = true;
                    this.emit('reconnect'); break;
                case 2:
                    this.connected = false;
                    this.emit('disconnect'); break;
                case 3:
                    this.emit('message', res.topic, res.payload); break;
                default: break;
            }
        }.bind(this));
    }

    subscribe(options) {
        if (!this.mqttInstance || !options || !options.topic) {
            throw new Error('mqtt not init or options invalid');
        }

        if (this.connected === false) {
            this.emit('error', 'subscirbe fail: not connected');
            return;
        }

        var ret = MQTT.subscribe(this.mqttInstance, options.topic, options.qos || 0);
        if (ret < 0) {
            if (typeof options.fail === 'function') {
                options.fail();
            }
            this.emit('error', 'subscribe error');
        }
        else {
            if (typeof options.success === 'function') {
                options.success();
            }
        }
    }

    unsubscribe(options) {
        if (!this.mqttInstance || !options || !options.topic) {
            throw new Error('mqtt not init or mqtt topic is invalid');
        }

        if (this.connected === false) {
            this.emit('error', 'unsubscribe fail: not connected');
            return;
        }

        var ret = MQTT.unsubscribe(this.mqttInstance, options.topic);
        if (ret < 0) {
            if (typeof options.fail === 'function') {
                options.fail();
            }
            this.emit('error', 'unsubscribe error');
            return
        }

        if (typeof options.success === 'function') {
            options.success();
        }
    }

    publish(options) {
        if (!this.mqttInstance || !options || !options.topic || !options.message) {
            throw new Error('mqtt not init or options invalid');
        }

        if (this.connected === false) {
            this.emit('error', 'publish fail: not connected');
            return;
        }

        MQTT.publish(this.mqttInstance, options.topic, options.message, options.qos || 0, function (ret) {
            if (ret < 0) {
                if (typeof options.fail === 'function') {
                    options.fail();
                }
                this.emit('error', options.topic);
                return;
            }

            if (typeof options.success === 'function') {
                options.success();
            }
        }.bind(this));
    }

    close() {
        if (!this.mqttInstance) {
            throw new Error('mqtt not init');
        }
        MQTT.close(this.mqttInstance, function (ret) {
            if (ret != 0) {
                this.emit('error', 'mqtt client close error');
                return;
            }
            this.emit('close');
        }.bind(this));

    }
}

export function createClient(options) {
    return new MQTTClient(options);
}

'use strict';

import * as UDP from 'UDP'
import * as event from 'events'

class UDPClient extends event.EventEmitter {
    constructor() {
        super();
        this.udpClientInstance = UDP.createSocket();
        if(this.udpClientInstance < 0) {
            throw new Error("create socket error!");
        }
        this.localPort = 0;
    }

    bind(port) {
        this.localPort = port || 0;
        if(UDP.bind(this.udpClientInstance, this.localPort) < 0) {
            throw new Error("bind error");
        }
        this._onListening();
    };

    send(options) {
        if (!(options.message instanceof Array)) {
            throw new Error("udp send message is empty");
        }
        options.message = new Uint8Array(options.message).buffer;
        UDP.sendto(this.udpClientInstance, options, function(ret) {
            console.log("sendto callback " + ret);
            if (ret < 0) {
                this.emit('error', 'udp send error');
                if(options.fail) options.fail();
                return;
            }
            this.emit('send', 'udp send success');
            if(options.success) options.success();
        }.bind(this));
    };

    close() {
        var ret = UDP.close(this.udpClientInstance);
        if (ret < 0) {
            console.log('close udp socket faild');
            return;
        }
        this.emit('close', 'udp client close');
    };

    _onListening() {
            UDP.recvfrom(this.udpClientInstance, function(data, rinfo, err) {
            console.log("recvfrom callback " + err);
            if (err === -4) {
                this.emit('error', 'udp client receive data error');
                return;
            }
            if (err > 0) {
                this.emit('message', data, rinfo);
            }
        }.bind(this));
    };
}

export function createSocket(options) {
    return new UDPClient(options);
}
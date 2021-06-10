'use strict';

const EventEmitter = require('events');

function stringToBytesArray(str) {
    var ch, st, re = [];
    for (var i = 0; i < str.length; i++ ) { 
        ch = str.charCodeAt(i);
        st = [];
       do {
            st.push(ch & 0xFF);
            ch = ch >> 8;
        }
        while ( ch );
        re = re.concat(st.reverse());
    }
    return re;
}

class UDPClient extends EventEmitter {
    constructor() {
        super();
        this.udpClientInstance = __native.UDP.createSocket();
        this.localPort = 0;
    }

    bind(port) {
        this.localPort = port || 0;
        if(__native.UDP.bind(this.udpClientInstance, this.localPort) < 0) {
            throw new Error("bind error");
        }
        this._onListening();
    };

    send(options) {
        if (!(options.message instanceof Array)) {
            options.message = stringToBytesArray(options.message);
        }
        __native.UDP.sendto(this.udpClientInstance, options, function(ret) {
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
        var ret = __native.UDP.close(this.udpClientInstance);
        if (ret < 0) {
            console.log('close udp socket faild');
            return;
        }
        this.emit('close', 'udp client close');
    };

    _onListening() {
        __native.UDP.recvfrom(this.udpClientInstance, function(data, rinfo, err) {
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

function createSocket(options) {
    return new UDPClient(options);
}

module.exports = {
    createSocket,
}
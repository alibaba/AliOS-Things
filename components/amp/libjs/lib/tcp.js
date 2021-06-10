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

class TCPClient extends EventEmitter{
    constructor(options) {
        super();
        if (!options || !options.host || !options.port) {
            throw new Error("invalid params");
        }
        this.options = {
            host: options.host,
            port: options.port
        }

        this.success = options.success || function(){};
        this.fail = options.fail || function(){};
        this._connect();
    }

    _connect() {
        this.connected = false;
        var cb = function(ret) {
            if (ret < 0) {
                this.fail();
                this.emit('error', 'tcp connect error');
                return;
            }
            this.tcpClientInstance = ret;
            this.connected = true;
            this.success();
            this.emit('connect');
            this._onListening();
        }
        if(__native.TCP.createSocket(this.options, cb.bind(this)) < 0){
            this.fail();
            this.emit('error', 'tcp connect error');
        }
    }

    send(options) {
        if (!(options.message instanceof Array)) {
            options.message = stringToBytesArray(options.message);
        }
        if(this.connected === false) {
            throw new Error("tcp not connected");
        }
        __native.TCP.send(this.tcpClientInstance, options.message, function(ret) {
            if (ret < 0) {
                this.emit('error', 'tcp send error');
                if(options.fail) options.fail();
                return;
            }
            this.emit('send', 'tcp send success');
            if(options.success) options.success();
        }.bind(this));
    };

    _onListening() {
        if (!this.tcpClientInstance) {
            throw new Error("tcpserver not init");
        }
        __native.TCP.recv(this.tcpClientInstance, function(datalen, data) {
            if (datalen === -2) {
                this.connected = false;
                this.emit('error', 'tcp receive message error');
                return;
            }
            if (datalen === -1) {
                this.connected = false;
                this.emit('disconnect');
                return;
            }
            if (datalen > 0) {
                this.emit('message', data);
            }
        }.bind(this));
    };

    close() {
        if (!this.tcpClientInstance) {
            throw new Error("tcpserver not init");
        }
        var ret = __native.TCP.close(this.tcpClientInstance);
        if (ret != 0) {
            this.emit('error', 'tcp socket close error');
            return;
        }
        this.emit('close', 'tcp socket close success');
    };

    reconnect() {
        if (this.tcpClientInstance) {
            __native.TCP.close(this.tcpClientInstance);
        }
        this._connect();
    };
}

function createClient(options) {
    return new TCPClient(options);
}

module.exports = {
    createClient,
}
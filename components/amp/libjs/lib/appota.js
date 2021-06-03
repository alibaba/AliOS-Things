'use strict';

const EventEmitter = require('events');

class UTILS_APPOTA extends EventEmitter {
    constructor(options){
        super();

        if (!options) {
            throw new Error('options invalid');
        }

        this._init(options);
    }

    _init(options) {
        __native.APPOTA.otaInit(options, function(res) {
            this.emit('new', res);
        }.bind(this));
    }

    download(options, cb) {
        __native.APPOTA.otaDownload(options, cb);
    }

    verify(options, cb) {
        __native.APPOTA.otaVerify(options, cb);
    }

    report(options) {
        var res = __native.APPOTA.otaReport(options);
        if (res < 0) {
            this.emit('error');
        }
    }

    upgrade(options, cb) {
        __native.APPOTA.otaUpgrade(options, cb);
    }
}

function open(options) {
    return new UTILS_APPOTA(options);
}

module.exports = {
    open
}
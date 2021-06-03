import * as APPOTA from 'APPOTA'
import * as event from 'events'

class UTILS_APPOTA extends event.EventEmitter {
    constructor(options){
        super();

        if (!options) {
            throw new Error('options invalid');
        }

        this._init(options);
    }

    _init(options) {
        //console.log(iothandle)
        APPOTA.otaInit(options, function(res) {
            this.emit('new', res);
        }.bind(this));
    }

    download(options, cb) {
        APPOTA.otaDownload(options, cb);
    }

    verify(options, cb) {
        APPOTA.otaVerify(options, cb);
    }

    report(options) {
        var res = APPOTA.otaReport(options);
        if (res < 0) {
            this.emit('error');
        }
    }

    upgrade(options, cb) {
        APPOTA.otaUpgrade(options, cb);
    }
}

export function open(options) {
    return new UTILS_APPOTA(options);
}
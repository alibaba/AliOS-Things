'use strict';
const EventEmitter = require('events');

class PayboxClient extends EventEmitter{
    constructor(options){
        super();
        if(!options || !options.xpPrductKey || !options.xpDeviceSecret){
            throw new Error('device info error');
        }

        this.options = {
            mqttPrductKey: options.mqttPrductKey ? options.mqttPrductKey : 'null',
            supplierId: options.supplierId ? options.supplierId : 'null',
            itemId: options.itemId ? options.itemId : 'null',
            boxModel: options.boxModel ? options.boxModel : 'null',
            boxVersion: options.boxVersion ? options.boxVersion : 'null',
            xpPrductKey: options.xpPrductKey,
            xpPrductSecret: options.xpPrductSecret ? options.xpPrductSecret : 'null',
            xpDeviceSecret: options.xpDeviceSecret
        }
        this.fail = options.fail || function(){};
        this.success = options.success || function(){};
        this.opened = false;
        this.onFlag = false;
        this._open();
        this._on();
    }

    _open() {
        this.payboxInstance = __native.PAYBOX.open(this.options);
        if (this.payboxInstance < 0) {
            this.fail();
            return;
        }
        this.opened = true;
        this.success();
    }

    _on() {
        if (this.payboxInstance < 0) {
            throw new Error("PAYBOX not init");
        }

        __native.PAYBOX.on(this.payboxInstance, function(event, data){
            this.emit(event, data);
        }.bind(this));

    };

    close() {
        if(this.payboxInstance < 0){
            throw new Error('device not init...');
        }
        var ret = __native.PAYBOX.close(this.payboxInstance);
        if (ret != 0) {
            this.emit('error', 'paybox client close falied')
            return
        }
        this.opened = false;
        this.emit('close');
    }
}

function open(options){
    return new PayboxClient(options);
}

module.exports = {
    open,
}



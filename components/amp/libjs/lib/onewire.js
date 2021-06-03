'use strict';

class HW_ONEWIRE {
    constructor(options) {
        if (!options || !options.id) {
            throw new Error("options is invalid");
        }
        this.options = {
            id: options.id
        };

        this.success = options.success || function(){};
        this.fail = options.fail || function(){};
        this._open();
    }

    _open() {
        this.onewireinstance = __native.ONEWIRE.open(this.options.id);
        if (!this.onewireinstance) {
            this.fail();
            return;
        }
        this.success();
    }
    setspeed(standard) {
        if (!this.onewireinstance) {
            throw new Error("onewire not init");
        }
        __native.ONEWIRE.setspeed(this.onewireinstance, standard);
    }

    reset() {
        if (!this.onewireinstance) {
            throw new Error("onewire not init");
        }
        __native.ONEWIRE.reset(this.onewireinstance);
    }

    readByte() {
        if (!this.onewireinstance) {
            throw new Error("onewire not init");
        }
        return __native.ONEWIRE.readByte(this.onewireinstance);
    };

    writeByte(data) {
        if (!this.onewireinstance) {
            throw new Error("onewire not init");
        }
        return __native.ONEWIRE.writeByte(this.onewireinstance, data);
    };

    close() {
        if (!this.onewireinstance) {
            throw new Error("onewire not init");
        }
        __native.ONEWIRE.close(this.onewireinstance);
    }
}

function open(options) {
    return new HW_ONEWIRE(options);
}

module.exports = {
    open,
}
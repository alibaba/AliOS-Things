import * as ONEWIRE from 'ONEWIRE'

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
        this.onewireinstance = ONEWIRE.open(this.options.id);
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
        this.onewireinstance.setspeed(standard);
    }

    reset() {
        if (!this.onewireinstance) {
            throw new Error("onewire not init");
        }
        this.onewireinstance.reset();
    }

    readByte() {
        if (!this.onewireinstance) {
            throw new Error("onewire not init");
        }
        return this.onewireinstance.readByte();
    };

    writeByte(data) {
        if (!this.onewireinstance) {
            throw new Error("onewire not init");
        }
        return this.onewireinstance.writeByte(data);
    };

    close() {
        if (!this.onewireinstance) {
            throw new Error("onewire not init");
        }
        this.onewireinstance.close();
    }
}

export function open(options) {
    return new HW_ONEWIRE(options);
}

'use strict';

class HW_SPI {
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
        this.spiInstance = __native.SPI.open(this.options.id);
        if (!this.spiInstance) {
            this.fail();
            return;
        }
        this.success();
    }

    write(data) {
        if (!this.spiInstance || !data) {
            throw new Error("spi not init or params is invalid");
        }
        __native.SPI.write(this.spiInstance, data);
    }

    read(bytes) {
        if (!this.spiInstance || !bytes) {
            throw new Error("spi not init or params is invalid");
        }
        return __native.SPI.read(this.spiInstance, bytes);
    };

    readWrite(sendData, bytes) {
        if (!this.spiInstance || !sendData ||!bytes) {
            throw new Error("spi not init or params is invalid");
        }
        return __native.SPI.sendRecv(this.spiInstance, sendData, bytes);
    };

    close() {
        if (!this.spiInstance) {
            throw new Error("spi not init");
        }
        __native.SPI.close(this.spiInstance);
    };
}

function open(options) {
    return new HW_SPI(options);
}

module.exports = {
    open,
}
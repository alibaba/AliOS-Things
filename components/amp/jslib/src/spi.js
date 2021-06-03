import * as SPI from 'SPI'

function byteArrayToArrayBuffer(byteArray) {
    return new Uint8Array(byteArray).buffer
}

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
        this.spiInstance = SPI.open(this.options.id);
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
        this.spiInstance.write(byteArrayToArrayBuffer(data));
    }

    read(bytes) {
        if (!this.spiInstance || !bytes) {
            throw new Error("spi not init or params is invalid");
        }
        return Array.prototype.slice.call(new Uint8Array(this.spiInstance.read(bytes)));
    };

    readWrite(sendData, bytes) {
        if (!this.spiInstance || !sendData ||!bytes) {
            throw new Error("spi not init or params is invalid");
        }
        return Array.prototype.slice.call(new Uint8Array(this.spiInstance.sendRecv(byteArrayToArrayBuffer(sendData),bytes)));
    };

    close() {
        if (!this.spiInstance) {
            throw new Error("spi not init");
        }
        this.spiInstance.close(this.spiInstance);
    };
}

export function open(options) {
    return new HW_SPI(options);
}
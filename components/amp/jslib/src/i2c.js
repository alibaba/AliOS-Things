import * as I2C from 'I2C'

function byteArrayToArrayBuffer(byteArray) {
    return new Uint8Array(byteArray).buffer
}
class HW_I2C {
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
        this.i2cInstance = I2C.open(this.options.id);
        if (!this.i2cInstance) {
            this.fail();
            return;
        }
        this.success();
    }

    write(data) {
        if (!this.i2cInstance || !data) {
            throw new Error("i2c not init or params is invalid");
        }
        this.i2cInstance.write(byteArrayToArrayBuffer(data));
    }

    read(bytes) {
        if (!this.i2cInstance || !bytes) {
            throw new Error("i2c not init or params is invalid");
        }
        return Array.prototype.slice.call(new Uint8Array(this.i2cInstance.read(bytes)));
    };

    writeMem(memaddr, data) {
        if (!this.i2cInstance) {
            throw new Error("i2c not init or params is invalid");
        }
        this.i2cInstance.writeReg(memaddr, byteArrayToArrayBuffer(data));
    }

    readMem(memaddr, bytes) {
        if (!this.i2cInstance) {
            throw new Error("i2c not init or params is invalid");
        }
        return Array.prototype.slice.call(new Uint8Array(this.i2cInstance.readReg(memaddr, bytes)));
    };

    close() {
        if (!this.i2cInstance) {
            throw new Error("i2c not init");
        }
        this.i2cInstance.close();
    };
}

export function open(options) {
    return new HW_I2C(options);
}

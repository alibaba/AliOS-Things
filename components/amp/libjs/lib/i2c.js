'use strict';

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
        this.i2cInstance = __native.I2C.open(this.options.id);
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
        __native.I2C.write(this.i2cInstance, data);
    }

    read(bytes) {
        if (!this.i2cInstance || !bytes) {
            throw new Error("i2c not init or params is invalid");
        }
        return __native.I2C.read(this.i2cInstance, bytes);
    };

    writeMem(memaddr, data) {
        if (!this.i2cInstance) {
            throw new Error("i2c not init or params is invalid");
        }        
        __native.I2C.writeReg(this.i2cInstance, memaddr, data);
    }

    readMem(memaddr, bytes) {
        if (!this.i2cInstance) {
            throw new Error("i2c not init or params is invalid");
        }
        return __native.I2C.readReg(this.i2cInstance, memaddr, bytes);
    };

    close() {
        if (!this.i2cInstance) {
            throw new Error("i2c not init");
        }
        __native.I2C.close(this.i2cInstance);
    };
}

function open(options) {
    return new HW_I2C(options);
}

module.exports = {
    open,
}
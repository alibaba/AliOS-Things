class _DCMOTOR {
    constructor(gpioObj) {
        this.gpioInstance = gpioObj;
    };

    ctrl(onoff) {
	this.gpioInstance.writeValue(onoff);
    };
}

export function DCMOTOR(gpioObj) {
    return new _DCMOTOR(gpioObj);
}

class _IR {
    constructor(gpioObj) {
        this.gpioInstance = gpioObj;
    };

    irDetect() {
	var value = this.gpioInstance.readValue();
	return value;
    };
}

export function IR(gpioObj) {
    return new _IR(gpioObj);
}

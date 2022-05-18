class _Gas {
    constructor(adcObj) {
        this.adcInstance = adcObj;
    };

    getVoltage() {
	var value = this.adcInstance.readValue();
	return value;
    };
}

export function Gas(adcObj) {
    return new _Gas(adcObj);
}

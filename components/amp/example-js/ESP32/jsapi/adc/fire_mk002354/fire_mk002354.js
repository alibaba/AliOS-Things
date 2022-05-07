class _Fire {
    constructor(adcObj) {
        this.adcInstance = adcObj;
    };

    getVoltage() {
	var value = this.adcInstance.readValue();
	return value;
    };
}

export function Fire(adcObj) {
    return new _Fire(adcObj);
}

class _PhotoResistor {
    constructor(adcObj) {
        this.adcInstance = adcObj;
    };

    getLightness() {
	var value = this.adcInstance.readValue();
	return value;
    };
}

export function PhotoResistor(adcObj) {
    return new _PhotoResistor(adcObj);
}

class _Hcho {
    constructor(adcObj) {
        this.adcInstance = adcObj;
    };

    getPPM() {
	var min = 400;
	var max = 0;
	var value = 0;
	var total = 0;
	var analogVoltage = 0;
	var ppm = 0;

	for (var i = 0; i < 32; i++) {
	  var value = this.adcInstance.read();
	  //console.log('value is ' + value);

	  total = total + value;

	  if (min >= value) {
	    min = value;
	  }

	  if (max <= value) {
	    max = value;
	  }
	}
	//console.log('total is ' + total);
	//console.log('min is ' + min);
	//console.log('max is ' + max);


	analogVoltage = (total - min - max) / 30;
	analogVoltage = analogVoltage + 23;
	analogVoltage = analogVoltage / 1000.0;
	ppm = 3.125 * analogVoltage - 1.25; //linear relationship(0.4V for 0 ppm and 2V for 5ppm) 

	if (ppm < 0) {
	  ppm = 0;
	} 
	//console.log('analogVoltage is ' + analogVoltage);
	//console.log('ppm is ' + ppm);

	ppm = Math.round(ppm * 100) / 100;
	//console.log('ppm is ' + ppm);
	return ppm;
    };
}

export function Hcho(adcObj) {
    return new _Hcho(adcObj);
}

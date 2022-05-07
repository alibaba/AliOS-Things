class _SHT3X {
    constructor(i2cObj) {
        this.i2cInstance = i2cObj;
	var cmd = [0x30, 0x41];
	//console.log('init cmd = ' + cmd);
	this.i2cInstance.write(cmd);
	sleepMs(20);
    };

    getTempHumidity() {
        var sensorData = [0x0, 0x0];
        var cmd = [0x24, 0x0b];
        //console.log('get data cmd = ' + cmd);
        this.i2cInstance.write(cmd);
        sleepMs(20);

        var retval = this.i2cInstance.read(6);
        //console.log('get data retval = ' + retval);
        var temp = (retval[0] << 8) | (retval[1]);
        var humi = (retval[3] << 8) | (retval[4]);
        //console.log('get data temp = ' + temp);
        //console.log('get data humi = ' + humi);

        sensorData[0] = -45.0 + (temp) * 175.0 / (0xFFFF - 1);
        sensorData[1] = humi * 0.0015259022;
        //console.log('get sensorData[0] = ' + sensorData[0]);
        //console.log('get sensorData[1] = ' + sensorData[1]);

        return sensorData;
    };

    getTemperature() {
        var data = this.getTempHumidity();
        //console.log('getTemperature data = ' + data[0]);
        return data[0];
    };

    getHumidity() {
        var data = this.getTempHumidity();
        //console.log('getHumidity data = ' + data[1]);
        return data[1];
    };
}

export function SHT3X(i2cObj) {
    return new _SHT3X(i2cObj);
}

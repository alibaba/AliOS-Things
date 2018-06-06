
function DHT11() {
    this.isOn = 1;
    this.handle = GPIO.open('dht11');
};

var dht11 = new DHT11();


dht11.get = function(){

    var val = HW.getDht11(this.handle);

    console.log('CurrentHumidity=' + val[0]);
    console.log('CurrentTemperature=' + val[1]);

    return val;
};

module.exports.dht11 = dht11;
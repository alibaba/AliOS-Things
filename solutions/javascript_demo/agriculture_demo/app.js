var fanSwitch = 0;
var waterSwitch = 0;
var beepSwitch = 0;
var gpio = require('gpio');
var beep = gpio.open({
	id: 'D4'
});
var relay1 = gpio.open({
	id: 'D3'
});
var relay2 = gpio.open({
	id: 'D2'
});
// GPIO will ouput high in default
// Turn off beep and relay immediately
beep.writeValue(beepSwitch);
relay1.writeValue(waterSwitch);
relay2.writeValue(fanSwitch);
console.log('turn off beep and relay');
sleepMs(100);

var iot = require('iot');
var network = require('network');

var net = network.openNetWorkClient();

var productKey = 'yourproductkey';      /* your productKey */
var deviceName = 'yourdevicename';      /* your deviceName */
var deviceSecret = 'yourdevicesecret';  /* your deviceSecret */
var device;
var mqtt_connected = false;

var modbus = require('./modbus.js');

var deviceAddr = 1;
var timeout = 300;
var modbus_device = modbus(
  { id: 'UART2' },
  { id: 'D13', polarity: 1 },
  deviceAddr
);

var temperature = 25.0;
var pressure = 0.0;
var bmp280 = require('./bmp280.js');
bmp280.bmp280_init();

var adc = require('adc');
var pir = adc.open({
  id: 'ADC0',
  success: function() {
    console.log('adc: open adc1 success')
  },
  fail: function() {
    console.log('adc: open adc1 failed')
  }
});
var pirStatus = 0;
var pirTempStatus = 0;
var pirCount = 0;

//----------------------- GPS Start--------------------------------
var GPS = require("./gps.js");
var gps = new GPS();

var uart1 = require('uart');
// gnss uart
var gnss = uart1.open({
    id: 'UART1'
});

var geoLocation_data = {'lat':0, 'lon':0, 'alt':0}
gps.on("data", function (parsed) {
    //console.log(parsed);
    geoLocation_data['lat'] = gps.state["lat"];
    geoLocation_data['lon'] = gps.state["lon"];
    geoLocation_data['alt'] = gps.state["alt"];
    console.log("geo data " + JSON.stringify(geoLocation_data, null, 4))
});


function ArrayToString(fileData) {
    var dataString = "";
    for (var i = 0; i < fileData.length; i++) {
        dataString += String.fromCharCode(fileData[i]);
    }
    return dataString;
}

var GGA;
gnss.on('data', function(data) {
    var aaa = ArrayToString(data);
    var bbb = aaa.split("$");
    GGA = "$" + bbb[1];
});

// GPS data post
setInterval(function(){
    var gga_default = "$GNGGA,033122.000,3111.28510,N,12136.26122,E,1,13,1.0,1.3,M,11.7,M,,*4B"

    /* gga_default is temporary data for agriculture demo, we should use real 'GGA' data */
    gps.update(gga_default);
    //gps.update(GGA);
    if(mqtt_connected) {
        device.postProps(JSON.stringify({
            GeoLocation: {
                Longitude: geoLocation_data['lon'],
                Latitude:  geoLocation_data['lat'],
                Altitude:  geoLocation_data['alt'],
                CoordinateSystem: 1
                }
            }));
    }
}, 5000);
//----------------------- GPS End--------------------------------

setInterval(function(){
    temperature = bmp280.bmp280TemperatureRead();
    pressure = bmp280.bmp280PressureRead();
    pressure = pressure / 100; // iot needs hPa
    console.log('Temperature is ' + temperature + ' Pressure is ' + pressure);
    if(mqtt_connected) {
        device.postProps(JSON.stringify({
            CurrentTemperature: temperature
        }));
        device.postProps(JSON.stringify({
            Atmosphere: pressure
        }));
    }
}, 10000);

setInterval(function(){
    var pinStatus = (pir.readValue() > 512) ? 1 : 0;
    if(pirTempStatus == pinStatus) {
        pirCount++;
        if(pirCount > 4) {
            pirCount = 0;
            if(pirTempStatus != pirStatus) {
                pirStatus = pirTempStatus;
                if(mqtt_connected) {
                    device.postProps(JSON.stringify({
                        AlarmState: pirStatus
                    }));
                }
                console.log('Pir status is ' + pirStatus);
            }
        }
    } else {
        pirTempStatus = pinStatus;
        pirCount = 0;
    }
}, 50);

function createDevice() {
    device = iot.device({
        productKey: productKey,
        deviceName: deviceName,
        deviceSecret: deviceSecret
    });
    var humidity = 50;

    device.on('connect', function () {
        console.log('(re)connected');
        mqtt_connected = true;
        /* post props */
        device.postProps(JSON.stringify({
            Coil: fanSwitch, CurrentTemperature: temperature, WaterOutletSwitch: waterSwitch, Buzzer: beepSwitch,
            CurrentHumidity:humidity, AlarmState: pirStatus, Atmosphere: pressure, SoilTemperature: 25.5,
            SoilMoisture: 50.5
        }));

        /* 云端设置属性事件 */
        device.onProps(function (res) {
            console.log('cloud req msg_id is ' + res.msg_id);
            console.log('cloud req params_len is ' + res.params_len);
            console.log('cloud req params is ' + res.params);
            var payload = JSON.parse(res.params);
            if(payload.Coil !== undefined)
            {
                fanSwitch = parseInt(payload.Coil);
                relay2.writeValue(fanSwitch);
                device.postProps(JSON.stringify({
                    Coil: fanSwitch
                }));
            }
            if(payload.WaterOutletSwitch !== undefined)
            {
                waterSwitch = parseInt(payload.WaterOutletSwitch);
                relay1.writeValue(waterSwitch);
                device.postProps(JSON.stringify({
                    WaterOutletSwitch: waterSwitch
                }));
            }
            if(payload.Buzzer !== undefined)
            {
                beepSwitch = parseInt(payload.Buzzer);
                beep.writeValue(beepSwitch);
                device.postProps(JSON.stringify({
                    Buzzer: beepSwitch
                }));
            }
        });

        /* 云端下发服务事件 */
        device.onService(function (res) {
            console.log('received cloud msg_id is ' + res.msg_id);
            console.log('received cloud service_id is ' + res.service_id);
            console.log('received cloud params_len is ' + res.params_len);
            console.log('received cloud params is ' + res.params);
        });
    });

    /* 网络断开事件 */
    device.on('disconnect', function () {
        console.log('disconnect ');
        mqtt_connected = false;
    });

    /* 关闭连接事件 */
    device.on('end', function () {
        console.log('iot client just closed');
        mqtt_connected = false;
    });

    /* 发生错误事件 */
    device.on('error', function (err) {
        console.log('error ' + err);
    });
}

var status = net.getStatus();
console.log('net status is: ' + status);


if (status == 'connect') {
    createDevice();
} else {
    net.on('connect', function () {
        createDevice();
    });
}

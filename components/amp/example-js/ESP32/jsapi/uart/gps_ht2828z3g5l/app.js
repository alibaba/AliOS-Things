import * as uart from 'uart';
import * as gpsObj from './gps.js';
var gps = gpsObj.GPS();

// gnss uart
var gnss = uart.open({
    id: 'serial2',
    success: function() {
        console.log('gnss open success')
    },
    fail: function() {
        console.log('gnss open failed')
    }
});

var geoLocation_data = {'lat':0, 'lon':0, 'alt':0}
gps.on("data", function (parsed) {
    console.log(parsed);
    geoLocation_data['lat'] = parsed["lat"];
    geoLocation_data['lon'] = parsed["lon"];
    geoLocation_data['alt'] = parsed["alt"];
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
gnss.on('data', function(data, len) {
    console.log('gnss data = ' + data)
    var aaa = data;
    var bbb = aaa.split("$");
    console.log('gnss bbb = ' + bbb)
    GGA = "$" + bbb[1];
    console.log('gnss GGA = ' + GGA)
    var gga_tag = GGA.split(",");
    console.log('gnss gga_tag = ' + gga_tag)
    console.log('gnss gga_tag[0] = ' + gga_tag[0])
    if (gga_tag[0] == '$GNGGA') {
        console.log('get GGA = ' + GGA)
        gps.update(GGA);
    } else {
        console.log('not get GGA = ' + GGA)
    }
});

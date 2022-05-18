/**
 * @license GPS.js v0.6.0 26/01/2016
 *
 * Copyright (c) 2016, Robert Eisele (robert@xarg.org)
 * Dual licensed under the MIT or GPL Version 2 licenses.
 **/

function parseCoord(coord, dir) {
      if (coord === '')
        return null;

      var n, sgn = 1;
      switch (dir) {
        case 'S':
          sgn = -1;
        case 'N':
          n = 2;
          break;
        case 'W':
          sgn = -1;
        case 'E':
          n = 3;
          break;
      }
      return sgn * (parseFloat(coord.slice(0, n)) + parseFloat(coord.slice(n)) / 60);
}

function parseDist(num, unit) {
      if (num === '') {
        return null;
      }
      if (unit === 'M' || unit === '') {
        return parseFloat(num);
      }
}

function GGAFunction(str, gga) {
        if (gga.length !== 16 && gga.length !== 14) {
          console.log("Invalid GGA length: " + str);
          return false;
        }
        return {
          'lat': parseCoord(gga[2], gga[3]),
          'lon': parseCoord(gga[4], gga[5]),
          'alt': parseDist(gga[9], gga[10])
        };
}
export function ParseData(line) {
      if (typeof line !== 'string')
        return false;
  
      var nmea = line.split(',');
      var last = nmea.pop();
      if (nmea.length < 2 || line.charAt(0) !== '$' || last.indexOf('*') === -1) {
        return false;
      }
  
      last = last.split('*');
      nmea.push(last[0]);
      nmea.push(last[1]);
      nmea[0] = nmea[0].slice(3);
      console.log('nmea[0] = ' + nmea[0])
      if (nmea[0] == 'GGA') {
        var data = GGAFunction(line, nmea);
        return data;
      }
      return false;
}

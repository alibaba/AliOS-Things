/**
 * @license GPS.js v0.6.0 26/01/2016
 *
 * Copyright (c) 2016, Robert Eisele (robert@xarg.org)
 * Dual licensed under the MIT or GPL Version 2 licenses.
 **/


(function (root) {

  'use strict';

  var D2R = Math.PI / 180;

  var collectSats = {};
  var lastSeenSat = {};

  function updateState(state, data) {

    // TODO: can we really use RMC time here or is it the time of fix?
    if (data['type'] === 'RMC' || data['type'] === 'GGA' || data['type'] === 'GLL' || data['type'] === 'GNS') {
      state['time'] = data['time'];
      state['lat'] = data['lat'];
      state['lon'] = data['lon'];
    }

    if (data['type'] === 'ZDA') {
      state['time'] = data['time'];
    }

    if (data['type'] === 'GGA') {
      state['alt'] = data['alt'];
    }

    if (data['type'] === 'RMC' /* || data['type'] === 'VTG'*/ ) {
      // TODO: is rmc speed/track really interchangeable with vtg speed/track?
      state['speed'] = data['speed'];
      state['track'] = data['track'];
    }

    if (data['type'] === 'GSA') {
      state['satsActive'] = data['satellites'];
      state['fix'] = data['fix'];
      state['hdop'] = data['hdop'];
      state['pdop'] = data['pdop'];
      state['vdop'] = data['vdop'];
    }

    if (data['type'] === 'GSV') {

      var now = new Date().getTime();

      var sats = data['satellites'];
      for (var i = 0; i < sats.length; i++) {
        var prn = sats[i].prn;
        lastSeenSat[prn] = now;
        collectSats[prn] = sats[i];
      }

      var ret = [];
      for (var prn in collectSats) {
        if (now - lastSeenSat[prn] < 3000) // Sats are visible for 3 seconds
          ret.push(collectSats[prn])
      }
      state['satsVisible'] = ret;
    }
  }

  /**
   *
   * @param {String} time
   * @param {String=} date
   * @returns {Date}
   */
  function parseTime(time, date) {

    if (time === '') {
      return null;
    }

    var ret = new Date;

    if (date) {

      var year = date.slice(4);
      var month = date.slice(2, 4) - 1;
      var day = date.slice(0, 2);

      if (year.length === 4) {
        ret.setUTCFullYear(Number(year), Number(month), Number(day));
      } else {
        // If we need to parse older GPRMC data, we should hack something like
        // year < 73 ? 2000+year : 1900+year
        // Since GPS appeared in 1973
        ret.setUTCFullYear(Number('20' + year), Number(month), Number(day));
      }
    }

    ret.setUTCHours(Number(time.slice(0, 2)));
    ret.setUTCMinutes(Number(time.slice(2, 4)));
    ret.setUTCSeconds(Number(time.slice(4, 6)));

    // Extract the milliseconds, since they can be not present, be 3 decimal place, or 2 decimal places, or other?
    var msStr = time.slice(7);
    var msExp = msStr.length;
    var ms = 0;
    if (msExp !== 0) {
      ms = parseFloat(msStr) * Math.pow(10, 3 - msExp);
    }
    ret.setUTCMilliseconds(Number(ms));

    return ret;
  }

  function parseCoord(coord, dir) {

    // Latitude can go from 0 to 90; longitude can go from -180 to 180.

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
    /*
     * Mathematically, but more expensive and not numerical stable:
     *
     * raw = 4807.038
     * deg = Math.floor(raw / 100)
     *
     * dec = (raw - (100 * deg)) / 60
     * res = deg + dec // 48.1173
     */
    return sgn * (parseFloat(coord.slice(0, n)) + parseFloat(coord.slice(n)) / 60);
  }

  function parseNumber(num) {

    if (num === '') {
      return null;
    }
    return parseFloat(num);
  }

  function parseKnots(knots) {

    if (knots === '')
      return null;

    return parseFloat(knots) * 1.852;
  }

  function parseGSAMode(mode) {

    switch (mode) {
      case 'M':
        return 'manual';
      case 'A':
        return 'automatic';
      case '':
        return null;
    }
    throw new Error('INVALID GSA MODE: ' + mode);
  }

  function parseGGAFix(fix) {

    if (fix === '')
      return null;

    switch (parseInt(fix, 10)) {
      case 0:
        return null;
      case 1:
        return 'fix'; // valid SPS fix
      case 2:
        return 'dgps-fix'; // valid DGPS fix
      case 3:
        return 'pps-fix'; // valid PPS fix
      case 4:
        return 'rtk'; // valid (real time kinematic) RTK fix
      case 5:
        return 'rtk-float'; // valid (real time kinematic) RTK float
      case 6:
        return 'estimated'; // dead reckoning
      case 7:
        return 'manual';
      case 8:
        return 'simulated';
    }
    throw new Error('INVALID GGA FIX: ' + fix);
  }

  function parseGSAFix(fix) {

    switch (fix) {
      case '1':
      case '':
        return null;
      case '2':
        return '2D';
      case '3':
        return '3D';
    }
    throw new Error('INVALID GSA FIX: ' + fix);
  }

  function parseRMC_GLLStatus(status) {

    switch (status) {
      case 'A':
        return 'active';
      case 'V':
        return 'void';
      case '':
        return null;
    }
    throw new Error('INVALID RMC/GLL STATUS: ' + status);
  }

  function parseFAA(faa) {

    // Only A and D will correspond to an Active and reliable Sentence

    switch (faa) {
      case '':
        return null;
      case 'A':
        return 'autonomous';
      case 'D':
        return 'differential';
      case 'E':
        return 'estimated'; // dead reckoning
      case 'M':
        return 'manual input';
      case 'S':
        return 'simulated';
      case 'N':
        return 'not valid';
      case 'P':
        return 'precise';
      case 'R':
        return 'rtk'; // valid (real time kinematic) RTK fix
      case 'F':
        return 'rtk-float'; // valid (real time kinematic) RTK float
    }
    throw new Error('INVALID FAA MODE: ' + faa);
  }

  function parseRMCVariation(vari, dir) {

    if (vari === '' || dir === '')
      return null;

    var q = (dir === 'W') ? -1.0 : 1.0;

    return parseFloat(vari) * q;
  }

  function isValid(str, crc) {

    var checksum = 0;
    for (var i = 1; i < str.length; i++) {
      var c = str.charCodeAt(i);

      if (c === 42) // Asterisk: *
        break;

      checksum ^= c;
    }
    return checksum === parseInt(crc, 16);
  }

  function parseDist(num, unit) {

    if (unit === 'M' || unit === '') {
      return parseNumber(num);
    }
    throw new Error('Unknown unit: ' + unit);
  }

  /**
   *
   * @constructor
   */
  function GPS() {

    if (!(this instanceof GPS)) {
      return new GPS;
    }

    this['events'] = {};
    this['state'] = {
      'errors': 0,
      'processed': 0
    };
  }

  GPS.prototype['events'] = null;
  GPS.prototype['state'] = null;

  GPS['mod'] = {
    // Global Positioning System Fix Data
    'GGA': function (str, gga) {

      if (gga.length !== 16 && gga.length !== 14) {
        throw new Error("Invalid GGA length: " + str);
      }

      /*
       11
       1         2       3 4        5 6 7  8   9  10 |  12 13  14  15
       |         |       | |        | | |  |   |   | |   | |   |   |
       $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
       
       1) Time (UTC)
       2) Latitude
       3) N or S (North or South)
       4) Longitude
       5) E or W (East or West)
       6) GPS Quality Indicator,
       0 = Invalid, 1 = Valid SPS, 2 = Valid DGPS, 3 = Valid PPS
       7) Number of satellites in view, 00 - 12
       8) Horizontal Dilution of precision, lower is better
       9) Antenna Altitude above/below mean-sea-level (geoid)
       10) Units of antenna altitude, meters
       11) Geoidal separation, the difference between the WGS-84 earth
       ellipsoid and mean-sea-level (geoid), '-' means mean-sea-level below ellipsoid
       12) Units of geoidal separation, meters
       13) Age of differential GPS data, time in seconds since last SC104
       type 1 or 9 update, null field when DGPS is not used
       14) Differential reference station ID, 0000-1023
       15) Checksum
       */

      return {
        'time': parseTime(gga[1]),
        'lat': parseCoord(gga[2], gga[3]),
        'lon': parseCoord(gga[4], gga[5]),
        'alt': parseDist(gga[9], gga[10]),
        'quality': parseGGAFix(gga[6]),
        'satellites': parseNumber(gga[7]),
        'hdop': parseNumber(gga[8]), // dilution
        'geoidal': parseDist(gga[11], gga[12]), // aboveGeoid
        'age': gga[13] === undefined ? null : parseNumber(gga[13]), // dgps time since update
        'stationID': gga[14] === undefined ? null : parseNumber(gga[14]) // dgpsReference??
      };
    },
    // GPS DOP and active satellites
    'GSA': function (str, gsa) {

      if (gsa.length !== 19 && gsa.length !== 20) {
        throw new Error('Invalid GSA length: ' + str);
      }

      /*
       eg1. $GPGSA,A,3,,,,,,16,18,,22,24,,,3.6,2.1,2.2*3C
       eg2. $GPGSA,A,3,19,28,14,18,27,22,31,39,,,,,1.7,1.0,1.3*35
       
       
       1    = Mode:
       M=Manual, forced to operate in 2D or 3D
       A=Automatic, 3D/2D
       2    = Mode:
       1=Fix not available
       2=2D
       3=3D
       3-14 = PRNs of Satellite Vehicles (SVs) used in position fix (null for unused fields)
       15   = PDOP
       16   = HDOP
       17   = VDOP
       (18) = systemID NMEA 4.10
       18   = Checksum
       */

      var sats = [];
      for (var i = 3; i < 15; i++) {

        if (gsa[i] !== '') {
          sats.push(parseInt(gsa[i], 10));
        }
      }

      return {
        'mode': parseGSAMode(gsa[1]),
        'fix': parseGSAFix(gsa[2]),
        'satellites': sats,
        'pdop': parseNumber(gsa[15]),
        'hdop': parseNumber(gsa[16]),
        'vdop': parseNumber(gsa[17]),
        'systemId': gsa.length > 19 ? parseNumber(gsa[18]) : null
      };
    },
    // Recommended Minimum data for gps
    'RMC': function (str, rmc) {

      if (rmc.length !== 13 && rmc.length !== 14 && rmc.length !== 15) {
        throw new Error('Invalid RMC length: ' + str);
      }

      /*
       $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
       
       RMC  = Recommended Minimum Specific GPS/TRANSIT Data
       1    = UTC of position fix
       2    = Data status (A-ok, V-invalid)
       3    = Latitude of fix
       4    = N or S
       5    = Longitude of fix
       6    = E or W
       7    = Speed over ground in knots
       8    = Track made good in degrees True
       9    = UT date
       10   = Magnetic variation degrees (Easterly var. subtracts from true course)
       11   = E or W
       (12) = NMEA 2.3 introduced FAA mode indicator (A=Autonomous, D=Differential, E=Estimated, N=Data not valid)
       (13) = NMEA 4.10 introduced nav status
       12   = Checksum
       */

      return {
        'time': parseTime(rmc[1], rmc[9]),
        'status': parseRMC_GLLStatus(rmc[2]),
        'lat': parseCoord(rmc[3], rmc[4]),
        'lon': parseCoord(rmc[5], rmc[6]),
        'speed': parseKnots(rmc[7]),
        'track': parseNumber(rmc[8]), // heading
        'variation': parseRMCVariation(rmc[10], rmc[11]),
        'faa': rmc.length > 13 ? parseFAA(rmc[12]) : null,
        'navStatus': rmc.length > 14 ? rmc[13] : null
      };
    },
    // Track info
    'VTG': function (str, vtg) {

      if (vtg.length !== 10 && vtg.length !== 11) {
        throw new Error('Invalid VTG length: ' + str);
      }

      /*
       ------------------------------------------------------------------------------
       1  2  3  4  5  6  7  8 9   10
       |  |  |  |  |  |  |  | |   |
       $--VTG,x.x,T,x.x,M,x.x,N,x.x,K,m,*hh<CR><LF>
       ------------------------------------------------------------------------------
       
       1    = Track made good (degrees true)
       2    = Fixed text 'T' indicates that track made good is relative to true north
       3    = optional: Track made good (degrees magnetic)
       4    = optional: M: track made good is relative to magnetic north
       5    = Speed over ground in knots
       6    = Fixed text 'N' indicates that speed over ground in in knots
       7    = Speed over ground in kilometers/hour
       8    = Fixed text 'K' indicates that speed over ground is in kilometers/hour
       (9)   = FAA mode indicator (NMEA 2.3 and later)
       9/10 = Checksum
       */

      if (vtg[2] === '' && vtg[8] === '' && vtg[6] === '') {

        return {
          'track': null,
          'trackMagetic': null,
          'speed': null,
          'faa': null
        };
      }

      if (vtg[2] !== 'T') {
        throw new Error('Invalid VTG track mode: ' + str);
      }

      if (vtg[8] !== 'K' || vtg[6] !== 'N') {
        throw new Error('Invalid VTG speed tag: ' + str);
      }

      return {
        'track': parseNumber(vtg[1]), // heading
        'trackMagnetic': vtg[3] === '' ? null : parseNumber(vtg[3]), // heading uncorrected to magnetic north
        'speed': parseKnots(vtg[5]),
        'faa': vtg.length === 11 ? parseFAA(vtg[9]) : null
      };
    },
    // satellites in view
    'GSV': function (str, gsv) {

      if (gsv.length % 4 % 3 === 0) {
        throw new Error('Invalid GSV length: ' + str);
      }

      /*
       $GPGSV,1,1,13,02,02,213,,03,-3,000,,11,00,121,,14,13,172,05*67
       
       1    = Total number of messages of this type in this cycle
       2    = Message number
       3    = Total number of SVs in view
       repeat [
       4    = SV PRN number
       5    = Elevation in degrees, 90 maximum
       6    = Azimuth, degrees from true north, 000 to 359
       7    = SNR (signal to noise ratio), 00-99 dB (null when not tracking, higher is better)
       ]
       N+1   = signalID NMEA 4.10
       N+2   = Checksum
       */

      var sats = [];

      for (var i = 4; i < gsv.length - 3; i += 4) {

        var prn = parseNumber(gsv[i]);
        var snr = parseNumber(gsv[i + 3]);
        /*
         Plot satellites in Radar chart with north on top
         by linear map elevation from 0° to 90° into r to 0
         
         centerX + cos(azimuth - 90) * (1 - elevation / 90) * radius
         centerY + sin(azimuth - 90) * (1 - elevation / 90) * radius
         */
        sats.push({
          'prn': prn,
          'elevation': parseNumber(gsv[i + 1]),
          'azimuth': parseNumber(gsv[i + 2]),
          'snr': snr,
          'status': prn !== null ? (snr !== null ? 'tracking' : 'in view') : null
        });
      }

      return {
        'msgNumber': parseNumber(gsv[2]),
        'msgsTotal': parseNumber(gsv[1]),
        //'satsInView'  : parseNumber(gsv[3]), // Can be obtained by satellites.length
        'satellites': sats,
        'signalId': gsv.length % 4 === 2 ? parseNumber(gsv[gsv.length - 2]) : null // NMEA 4.10 addition
      };
    },
    // Geographic Position - Latitude/Longitude
    'GLL': function (str, gll) {

      if (gll.length !== 9 && gll.length !== 8) {
        throw new Error('Invalid GLL length: ' + str);
      }

      /*
       ------------------------------------------------------------------------------
       1       2 3        4 5         6 7   8
       |       | |        | |         | |   |
       $--GLL,llll.ll,a,yyyyy.yy,a,hhmmss.ss,a,m,*hh<CR><LF>
       ------------------------------------------------------------------------------
       
       1. Latitude
       2. N or S (North or South)
       3. Longitude
       4. E or W (East or West)
       5. Universal Time Coordinated (UTC)
       6. Status A - Data Valid, V - Data Invalid
       7. FAA mode indicator (NMEA 2.3 and later)
       8. Checksum
       */

      return {
        'time': parseTime(gll[5]),
        'status': parseRMC_GLLStatus(gll[6]),
        'lat': parseCoord(gll[1], gll[2]),
        'lon': parseCoord(gll[3], gll[4]),
        'faa': gll.length === 9 ? parseFAA(gll[7]) : null
      };
    },
    // UTC Date / Time and Local Time Zone Offset
    'ZDA': function (str, zda) {

      /*
       1    = hhmmss.ss = UTC
       2    = xx = Day, 01 to 31
       3    = xx = Month, 01 to 12
       4    = xxxx = Year
       5    = xx = Local zone description, 00 to +/- 13 hours
       6    = xx = Local zone minutes description (same sign as hours)
       */

      // TODO: incorporate local zone information

      return {
        'time': parseTime(zda[1], zda[2] + zda[3] + zda[4])
        //'delta': time === null ? null : (Date.now() - time) / 1000
      };
    },
    'GST': function (str, gst) {

      if (gst.length !== 10) {
        throw new Error('Invalid GST length: ' + str);
      }

      /*
       1    = Time (UTC)
       2    = RMS value of the pseudorange residuals; includes carrier phase residuals during periods of RTK (float) and RTK (fixed) processing
       3    = Error ellipse semi-major axis 1 sigma error, in meters
       4    = Error ellipse semi-minor axis 1 sigma error, in meters
       5    = Error ellipse orientation, degrees from true north
       6    = Latitude 1 sigma error, in meters
       7    = Longitude 1 sigma error, in meters
       8    = Height 1 sigma error, in meters
       9    = Checksum
       */

      return {
        'time': parseTime(gst[1]),
        'rms': parseNumber(gst[2]),
        'ellipseMajor': parseNumber(gst[3]),
        'ellipseMinor': parseNumber(gst[4]),
        'ellipseOrientation': parseNumber(gst[5]),
        'latitudeError': parseNumber(gst[6]),
        'longitudeError': parseNumber(gst[7]),
        'heightError': parseNumber(gst[8])
      };
    },

    // add HDT
    'HDT': function (str, hdt) {

      if (hdt.length !== 4) {
        throw new Error('Invalid HDT length: ' + str);
      }

      /*
       ------------------------------------------------------------------------------
       1      2  3
       |      |  |
       $--HDT,hhh.hhh,T*XX<CR><LF>
       ------------------------------------------------------------------------------
       
       1. Heading in degrees
       2. T: indicates heading relative to True North
       3. Checksum
       */

      return {
        'heading': parseFloat(hdt[1]),
        'trueNorth': hdt[2] === 'T'
      };
    },

    'GRS': function (str, grs) {

      if (grs.length !== 18) {
        throw new Error('Invalid GRS length: ' + str);
      }

      var res = [];
      for (var i = 3; i <= 14; i++) {
        var tmp = parseNumber(grs[i]);
        if (tmp !== null)
          res.push(tmp);
      }

      return {
        'time': parseTime(grs[1]),
        'mode': parseNumber(grs[2]),
        'res': res
      };
    },
    'GBS': function (str, gbs) {

      if (gbs.length !== 10 && gbs.length !== 12) {
        throw new Error('Invalid GBS length: ' + str);
      }

      /*
       0      1   2   3   4   5   6   7   8
       |      |   |   |   |   |   |   |   |
       $--GBS,hhmmss.ss,x.x,x.x,x.x,x.x,x.x,x.x,x.x*hh<CR><LF>
       
       1. UTC time of the GGA or GNS fix associated with this sentence
       2. Expected error in latitude (meters)
       3. Expected error in longitude (meters)
       4. Expected error in altitude (meters)
       5. PRN (id) of most likely failed satellite
       6. Probability of missed detection for most likely failed satellite
       7. Estimate of bias in meters on most likely failed satellite
       8. Standard deviation of bias estimate
       --
       9. systemID (NMEA 4.10)
       10. signalID (NMEA 4.10)
       */

      return {
        'time': parseTime(gbs[1]),
        'errLat': parseNumber(gbs[2]),
        'errLon': parseNumber(gbs[3]),
        'errAlt': parseNumber(gbs[4]),
        'failedSat': parseNumber(gbs[5]),
        'probFailedSat': parseNumber(gbs[6]),
        'biasFailedSat': parseNumber(gbs[7]),
        'stdFailedSat': parseNumber(gbs[8]),
        'systemId': gbs.length === 12 ? parseNumber(gbs[9]) : null,
        'signalId': gbs.length === 12 ? parseNumber(gbs[10]) : null
      };
    },
    'GNS': function (str, gns) {

      if (gns.length !== 14 && gns.length !== 15) {
        throw new Error('Invalid GNS length: ' + str);
      }

      return {
        'time': parseTime(gns[1]),
        'lat': parseCoord(gns[2], gns[3]),
        'lon': parseCoord(gns[4], gns[5]),
        'mode': gns[6],
        'satsUsed': parseNumber(gns[7]),
        'hdop': parseNumber(gns[8]),
        'alt': parseNumber(gns[9]),
        'sep': parseNumber(gns[10]),
        'diffAge': parseNumber(gns[11]),
        'diffStation': parseNumber(gns[12]),
        'navStatus': gns.length === 15 ? gns[13] : null // NMEA 4.10
      };
    }
  };

  GPS['Parse'] = function (line) {

    if (typeof line !== 'string')
      return false;

    var nmea = line.split(',');

    var last = nmea.pop();

    // HDT is 2 items length
    if (nmea.length < 2 || line.charAt(0) !== '$' || last.indexOf('*') === -1) {
      return false;
    }

    last = last.split('*');
    nmea.push(last[0]);
    nmea.push(last[1]);

    // Remove $ character and first two chars from the beginning
    nmea[0] = nmea[0].slice(3);

    if (GPS['mod'][nmea[0]] !== undefined) {
      // set raw data here as well?
      var data = this['mod'][nmea[0]](line, nmea);
      data['raw'] = line;
      data['valid'] = isValid(line, nmea[nmea.length - 1]);
      data['type'] = nmea[0];

      return data;
    }
    return false;
  };

  // Heading (N=0, E=90, S=189, W=270) from point 1 to point 2
  GPS['Heading'] = function (lat1, lon1, lat2, lon2) {

    var dlon = (lon2 - lon1) * D2R;

    lat1 = lat1 * D2R;
    lat2 = lat2 * D2R;

    var sdlon = Math.sin(dlon);
    var cdlon = Math.cos(dlon);

    var slat1 = Math.sin(lat1);
    var clat1 = Math.cos(lat1);

    var slat2 = Math.sin(lat2);
    var clat2 = Math.cos(lat2);

    var y = sdlon * clat2;
    var x = clat1 * slat2 - slat1 * clat2 * cdlon;

    var head = Math.atan2(y, x) * 180 / Math.PI;

    return (head + 360) % 360;
  };

  GPS['Distance'] = function (lat1, lon1, lat2, lon2) {

    // Haversine Formula
    // R.W. Sinnott, "Virtues of the Haversine", Sky and Telescope, vol. 68, no. 2, 1984, p. 159

    // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
    // var RADIUS = 6371; // Earth radius average
    // var RADIUS = 6378.137; // Earth radius at equator
    var RADIUS = 6372.8; // Earth radius in km

    var hLat = (lat2 - lat1) * D2R * 0.5; // Half of lat difference
    var hLon = (lon2 - lon1) * D2R * 0.5; // Half of lon difference

    lat1 = lat1 * D2R;
    lat2 = lat2 * D2R;

    var shLat = Math.sin(hLat);
    var shLon = Math.sin(hLon);
    var clat1 = Math.cos(lat1);
    var clat2 = Math.cos(lat2);

    var tmp = shLat * shLat + clat1 * clat2 * shLon * shLon;

    //return RADIUS * 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1.0 - a));
    return RADIUS * 2 * Math.asin(Math.sqrt(tmp));
  };

  GPS['TotalDistance'] = function (path) {

    if (path.length < 2)
      return 0;

    var len = 0;
    for (var i = 0; i < path.length - 1; i++) {
      var c = path[i];
      var n = path[i + 1];
      len += GPS['Distance'](c['lat'], c['lon'], n['lat'], n['lon']);
    }
    return len;
  };

  GPS.prototype['update'] = function (line) {

    var parsed = GPS['Parse'](line);

    this['state']['processed']++;

    if (parsed === false) {
      this['state']['errors']++;
      return false;
    }

    updateState(this['state'], parsed);

    this['emit']('data', parsed);
    this['emit'](parsed.type, parsed);

    return true;
  };

  GPS.prototype['partial'] = "";

  GPS.prototype['updatePartial'] = function (chunk) {

    this['partial'] += chunk;

    while (true) {

      var pos = this['partial'].indexOf("\r\n");

      if (pos === -1)
        break;

      var line = this['partial'].slice(0, pos);

      if (line.charAt(0) === '$') {
        this['update'](line);
      }
      this['partial'] = this['partial'].slice(pos + 2);
    }
  };

  GPS.prototype['on'] = function (ev, cb) {

    if (this['events'][ev] === undefined) {
      this['events'][ev] = cb;
      return this;
    }
    return null;
  };

  GPS.prototype['off'] = function (ev) {

    if (this['events'][ev] !== undefined) {
      this['events'][ev] = undefined;
    }
    return this;
  };

  GPS.prototype['emit'] = function (ev, data) {
    if (this['events'][ev] !== undefined) {
      this['events'][ev].call(this, data);
    }
  };

  if (typeof exports === 'object') {
    Object.defineProperty(exports, "__esModule", {
      'value': true
    });
    GPS['default'] = GPS;
    GPS['GPS'] = GPS;
    module['exports'] = GPS;
  } else {
    root['GPS'] = GPS;
  }

})(this);
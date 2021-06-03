'use strict';

class ADVANCED_LOCATION {
    constructor() {}

    getAccessApInfo() {
        var ap_info; 
        ap_info = __native.LOCATION.accessedWifi();
        return ap_info;
    }

    getScannedApInfo() {
        var ap_info;
        ap_info = __native.LOCATION.scannedWifi();
        return ap_info;
    }

    getAccessedLbsInfo() {
        var lbs_info;
        lbs_info = __native.LOCATION.accessedLbs();
        return lbs_info;
    }

    getNearLbsInfo() {
        var near_lbs_info;
        near_lbs_info = __native.LOCATION.nearbts();
        return near_lbs_info;
    }

}

function open() {
    return new ADVANCED_LOCATION();
}

module.exports = {
    open,
}
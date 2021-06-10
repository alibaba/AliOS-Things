'use strict';

function start() {
    __native.BLECFGNET.start();
}

function recoveryWifi() {
    __native.BLECFGNET.recoveryWifi();
}

function recoveryDevInfo() {
    __native.BLECFGNET.recoveryDevInfo();
}

module.exports = {
    start,
    recoveryWifi,
    recoveryDevInfo
}
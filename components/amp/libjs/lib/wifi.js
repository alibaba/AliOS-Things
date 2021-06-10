'use strict';

const event = require('events');

function setIfConfig(options) {
    if (!options) {
        throw new Error('params is invalid');
    }
    __native.WIFI.set_ifconfig(options);
}

function getIfConfig() {
    var config = __native.WIFI.get_ifconfig();
    if (!config) {
        throw 'get ifconfig failed';
    }
    return config;
}

function connect(options) {
    if(!options) {
        throw new Error("invalid params");
    }
    __native.WIFI.connect(options, function(state) {
        if (state == 1) {
            console.log('wifi connected success')
            event.emit('connect');
        }
    });
}

function disconnect() {
    return __native.WIFI.disconnect();
}

function getState() {
    var state;
    state =  __native.WIFI.getState();
    return state;
}

module.exports = {
    setIfConfig,
    getIfConfig,
    connect,
    disconnect,
    getState
}
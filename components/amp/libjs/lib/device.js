'use strict';

var kv = require('kv');
function setDeviceInfo(pk, ps) {
    if (!pk || !ps) {
        throw new Error('params is invalid');
    }
    
    var productkey_key = '_amp_internal_productkey';
    var productsecret_key = '_amp_internal_productsecret';

    kv.setStorageSync(productkey_key, pk);
    kv.setStorageSync(productsecret_key, ps);
}

function setToken(token) {
    if(!token) {
        throw new Error("invalid params");
    }

    var token_key = '_amp_device_token';
    kv.setStorageSync(token_key, token);
}

function getToken() {
    return kv.getStorageSync('_amp_device_token');
}

module.exports = {
    setDeviceInfo,
    setToken,
    getToken
}
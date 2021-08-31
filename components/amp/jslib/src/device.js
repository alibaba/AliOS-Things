import * as kv from 'kv';
export function setDeviceInfo(pk, ps) {
    if (!pk || !ps) {
        throw new Error('params is invalid');
    }

    var productkey_key = '_amp_internal_productkey';
    var productsecret_key = '_amp_internal_productsecret';

    kv.setStorageSync(productkey_key, pk);
    kv.setStorageSync(productsecret_key, ps);
}

export function setToken(token) {
    if(!token) {
        throw new Error("invalid params");
    }

    var token_key = '_amp_device_token';
    kv.setStorageSync(token_key, token);
}

export function getToken() {
    return kv.getStorageSync('_amp_device_token');
}

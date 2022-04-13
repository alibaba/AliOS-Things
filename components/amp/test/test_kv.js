/* 本测试case，测试kv的接口，接口返回值等 */
console.log('testing kv...');
// 测试 kv 模块
var kv = require('kv');
if(!(kv && kv.setStorageSync && kv.getStorageSync && kv.removeStorageSync)){
    throw new Error("[failed]  require(\'kv\')");
}
console.log('[success] require(\'kv\')');

// 测试 kv 写入
if(kv.setStorageSync('aiot', 'amp')){
    throw new Error("[failed] kv.setStorageSync()");
}
console.log("[success] kv.setStorageSync()");

// 测试 kv 读取
kv.getStorageSync('aiot');
var value = kv.getStorageSync('aiot');
if(!value || value !== 'amp'){
    throw new Error("[failed] kv.getStorageSync()");
}
if(kv.getStorageSync('aiot1') == 0){
    throw new Error("[failed] kv.getStorageSync()");
}
console.log("[success] kv.getStorageSync()");

// 测试 kv 修改
if(kv.setStorageSync('aiot', 'alios-things')){
    throw new Error("[failed] kv modify");
}
var value2 = kv.getStorageSync('aiot');
if(!value2 || value2 !== 'alios-things'){
    throw new Error("[failed] kv modify");
}
console.log("[success] kv modify");

// 测试 kv 清除
if(kv.removeStorageSync('aiot') || kv.getStorageSync('aiot') !== undefined){
    throw new Error("[failed] kv.removeStorageSync()");
}
console.log("[success] kv.removeStorageSync()");
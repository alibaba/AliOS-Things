/* 本测试case，测试fs的接口，接口返回值等 */
console.log('testing fs...');
// 测试 fs 模块
var fs = require('fs');
if(!(fs && fs.writeSync && fs.readSync && fs.unlinkSync)){
    throw new Error("[failed]  require(\'fs\')");
}
console.log('[success] require(\'fs\')');

// 测试 fs 写入
if(fs.writeSync('./aiot.txt', 'amp')){
    throw new Error("[failed] fs.writeSync()");
}
console.log("[success] fs.writeSync()");

// 测试 fs 读取
fs.readSync('./aiot.txt');
var value = fs.readSync('./aiot.txt');
if(!value || value !== 'amp'){
    throw new Error("[failed] fs.readSync()");
}

function testWrongFile() {
    try{
        fs.readSync('./aiot1.txt')
    } catch (e) {
        return true;
    } 
    throw new Error("[failed] fs.readSync()");
}

if (testWrongFile()) {
    console.log("[success] fs.readSync()");
}
// 测试 fs 清除
if(fs.unlinkSync('./aiot.txt') || fs.unlinkSync('./aiot.txt') !== undefined){
    throw new Error("[failed] fs.unlinkSync()");
}
console.log("[success] fs.unlinkSync()");
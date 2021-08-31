function Modbus(uartPort, dirPort) {
    var rtn = {}
    var lastTid = 1
    rtn.readTimeout = 500;
    rtn.readTimeouthandler;
    rtn.packetBufferLength = 100
    rtn.packets = []

    rtn.stream = Serial(uartPort, dirPort)
    rtn.protocal = 'rtu'

    rtn.read = function (unitId, address, timeout, callback) {
        var parsedAddress = parseAddress(address)
        var funcCode = parsedAddress.fcRead
        var length = parsedAddress.length
        address = parsedAddress.address

        rtn.readTimeout = timeout
        rtn.readTimeouthandler = setTimeout(function() {
            rtn.packets[tid].promiseReject('timeout')
        }, rtn.readTimeout)

        var tid = getTid()

        var buff = makeDataPacket(tid, 0, unitId, funcCode, address, null, length)
        if (rtn.protocal == 'rtu') { buff = buff.rtu }

        var packet = {
            onResponce: callback,
            tx: {
                funcCode: funcCode,
                tid: tid,
                address: address,
                hex: buff.toString('hex')
            },
            promiseResolve: null,
            promiseReject: null,
            rx: null
        }

        rtn.packets[tid] = packet

        rtn.stream.send(buff)
        console.log('read send ' + buff.toString('hex'))
        return new Promise(function (resolve, reject) {
            rtn.packets[tid].promiseResolve = resolve
            rtn.packets[tid].promiseReject = reject
        })

    }
    rtn.write = function (unitId, address, value, timeout, callback) {
        var parsedAddress = parseAddress(address)
        var funcCode = parsedAddress.fcWrite
        var length = parsedAddress.length
        address = parsedAddress.address
        var tid = getTid()

        rtn.readTimeout = timeout
        rtn.readTimeouthandler = setTimeout(function() {
            rtn.packets[tid].promiseReject('timeout')
        }, rtn.readTimeout)
    
        if (funcCode == 5 && value == true) { value = 65280 } // To force a coil on you send FF00 not 0001

        var buff = makeDataPacket(tid, 0, unitId, funcCode, address, value, length)
        if (rtn.protocal == 'rtu') { buff = buff.rtu }
        // console.log('buff ' + buff)
        var packet = {
            onResponce: callback,
            tx: {
                funcCode: funcCode,
                tid: tid,
                address: address,
                hex: buff.toString('hex')
            },
            rx: null
        }
        rtn.packets[tid] = packet

        rtn.stream.send(buff)
        console.log('write send ' + buff.toString('hex'))

        return new Promise(function (resolve, reject) {
            rtn.packets[tid].promiseResolve = resolve
            rtn.packets[tid].promiseReject = reject
        })
    }
    var getTid = function () {
        if (lastTid > rtn.packetBufferLength) { lastTid = 0 }
        lastTid++
        if (rtn.protocal == 'rtu') { lastTid = 0 }
        return lastTid
    }

    rtn.stream.onData = function (buf) {
        console.log('onData: ' + buf.toString('hex'))

        var modbusRes
        if (rtn.protocal == "rtu") { modbusRes = parseResponseRtu(buf) }

        var value = modbusRes.value
        var tid = modbusRes.tid
        if (rtn.protocal == "rtu") { tid = 0 }

        var err = null
        if (modbusRes.exceptionCode) { err = 'Exception Code: 02 - Illegal Data Address' }

        rtn.packets[tid].rx = modbusRes
        rtn.packets[tid].rx.hex = buf.toString('hex')
        if (typeof (rtn.packets[tid].onResponce) == "function") {
            rtn.packets[tid].onResponce(err, value)
        }

        if (err) {
            rtn.packets[tid].promiseReject(err)
        }
        else {
            // console.log('raw data:', value)
            rtn.packets[tid].promiseResolve(value)
            clearTimeout(rtn.readTimeouthandler)
        }

    }
    return rtn
}

function parseResponseRtu(buffer) {
    var res = {}
    res.tid = 1
    var buf = new Buffer(buffer)
    res.unitId = buf.readInt8(0)                     //Unit Id        - Byte 6
    res.funcCode = buf.readInt8(1)                     //Function Code  - Byte 7
    res.byteCount = Math.abs(buf.readInt8(2))           //Byte Count     - Byte 8
    if (buf.length > 3) {
        // res.value = buf.readIntBE(3, buf.length - 5)       //Data           - Bytes 9+
        // var arr = Array.prototype.slice.call(new Uint8Array(buf))
        // res.value = new Buffer(arr.slice(3, buf.length - 5))
        var dataBuffer = buf.subarray(3, buf.length - 2)
        res.value = Array.prototype.slice.call(new Uint8Array(buf))
    }

    return res
}

function crc16modbus(buf, previous) {
    var TABLE = [0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440, 0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40, 0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841, 0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40, 0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41, 0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641, 0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040, 0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240, 0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441, 0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41, 0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840, 0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40, 0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640, 0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041, 0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240, 0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441, 0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840, 0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41, 0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40, 0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640, 0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041, 0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241, 0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440, 0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40, 0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841, 0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40, 0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41, 0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040];

    var crc = typeof previous !== 'undefined' ? ~~previous : 0xffff;

    for (var index = 0; index < buf.length; index++) {
        var byte = buf[index];
        crc = (TABLE[(crc ^ byte) & 0xff] ^ crc >> 8) & 0xffff;
    }

    return crc;
}

function makeDataPacket(transId, protoId, unitId, funcCode, address, data, length) {

    if (typeof (data) == "boolean" && data) { data = 1 }
    if (typeof (data) == "boolean" && !data) { data = 0 }


    if (address == 0) { address = 65535 }
    else { address = address - 1 }

    var dataBytes = 0
    if (funcCode == 15) { dataBytes = length }
    if (funcCode == 16) { dataBytes = length * 2 }

    var bufferLength = 12
    if (funcCode == 15 || funcCode == 16) { bufferLength = 13 + dataBytes }

    var byteCount = bufferLength - 6

    var buf = new Buffer(bufferLength)

    buf.writeUInt16BE(transId, 0)
    buf.writeUInt16BE(protoId, 2)
    buf.writeUInt16BE(byteCount, 4)
    buf.writeUInt8(unitId, 6)
    buf.writeUInt8(funcCode, 7)
    buf.writeUInt16BE(address, 8)

    if (funcCode == 1 || funcCode == 2 || funcCode == 3 || funcCode == 4) {
        buf.writeUInt16BE(length, 10)
    }
    if (funcCode == 5 || funcCode == 6) {
        buf.writeInt16BE(data, 10)
    }
    if (funcCode == 15 || funcCode == 16) {
        buf.writeInt16BE(length, 10)
        buf.writeUInt8(dataBytes, 12)
        buf.writeInt32BE(data, 13)
    }

    var makeCrc = crc16modbus

    var bufRtu = buf.slice(6, bufferLength)
    var crc = makeCrc(bufRtu)
    var arr = Array.prototype.slice.call(new Uint8Array(bufRtu))
    arr.push(0);
    arr.push(0);
    var buffRtu = new Buffer(arr);
    buffRtu.writeUInt16LE(crc, buffRtu.length - 2)
    //console.log('buffRtu:', buffRtu);
    return { rtu: Array.prototype.slice.call(new Uint8Array(buffRtu)) }
}

function parseAddress(address) {
    var rtn = {}
    address = address.toLowerCase()

    // 提取地址
    // Data Type                  Short Hand   Size        Accessibility
    // Descrite Input             i            1 Bit       Read Only
    // Coil                       c            1 Bit       Read / Write
    // Input Register             ir           16 Bits     Read Only
    // Holding Register           hr           16 Bits     Read / Write
    var isRegister = address.replace('r', '').length !== address.length;
    if (isRegister) {
        rtn.address = address.substr(2)
        rtn.type = address.substr(0, 2)
    }
    if (!isRegister) {
        rtn.address = address.substr(1)
        rtn.type = address.substr(0, 1)
    }

    var isRange = rtn.address.replace('-', '').length !== rtn.address.length
    if (isRange) {
        var range = rtn.address.split('-')
        rtn.length = range[0] - range[1]
        if (rtn.length < 0) { rtn.address = range[0] }
        if (rtn.length > 0) { rtn.address = range[1] }
        rtn.length = Math.abs(rtn.length) + 1
    }
    if (!isRange) {
        rtn.length = 1
    }

    rtn.address = parseInt(rtn.address)

    // 获取类型
    // FC1 - Read Coil
    // FC2 - Read Input
    // FC3 - Read Holding Registers
    // FC4 - Read Input Registers
    // FC5 - Write Single Coil
    // FC6 - Write Single Register
    // FC15 - Write Multiple Coils
    // FC16 - Write Multiple Registers
    if (rtn.type == 'c') {
        rtn.fcRead = 1
        rtn.fcWrite = 5
    }
    if (rtn.type == 'i') {
        rtn.fcRead = 2
    }
    if (rtn.type == 'hr' && !isRange) {
        rtn.fcRead = 3
        rtn.fcWrite = 6
    }
    if (rtn.type == 'hr' && isRange) {
        rtn.fcRead = 3
        rtn.fcWrite = 16
    }
    if (rtn.type == 'ir') {
        rtn.fcRead = 4
    }

    return rtn
}

function Serial(uartPort, dirPort) {
    var rtn = {}
    var Uart = require('uart')

    rtn.port = Uart.open(uartPort)

    if (dirPort) {
        var events = require('events');
        var event = new events();
        var Gpio = require('gpio')
        var option = {};
        option.id = dirPort.id;
        rtn.io = Gpio.open(option);
    }

    rtn.send = function (data) {
        rtn.io.writeValue(dirPort.polarity === 0 ? 0 : 1);
        rtn.port.write(data)
        event.emit('send')
    }

    rtn.onData = function () { }

    rtn.port.on('data', function (data) {
        console.log("rtn onData");
        rtn.onData(data);
    })

    event.on('send', function () {
        rtn.io.writeValue(dirPort.polarity === 0 ? 1 : 0);
    })

    return rtn
}

module.exports = Modbus;
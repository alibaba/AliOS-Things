import * as spi from 'spi';

var W25Q = spi.open({
  id: "FLASH",
  success: function() {
    console.log('w25q flash open success')
  },
  fail: function() {
    console.log('w25q flash open failed')
  }
});

/**
 * 读取flash w25q128 的JEDEC ID值
 * 
 * @returns {objct} 返回 JEDEC 值值
 */
function getJEDEC() {
  /* 根据w25q数据手册，读取JEDEC ID要先发送命令 [0x90, 0, 0, 0] */
  var cmd = [0x90, 0, 0, 0];

  /* 根据w25q数据手册，要读取的ID长度为2，因此定义长度为2的 Unit8Array，元素值可以是任意值 */
  var val = [0xff, 0xff];

  /* 拼接数组 为 [0x90, 0, 0, 0, 0xff, 0xff] */
  var buf = cmd.concat(val);

  /* 发送长度为6 Byte的数据，同时读取6 Byte的数据 */
  val = W25Q.readWrite(buf, buf.length);

  var res = {};

  /* 提取制造商ID val[4] */
  res.manufacturerId = "0x" + val[4].toString(16).toLocaleUpperCase();

  /* 提取设备ID val[5] */
  res.deviceId = "0x" + val[5].toString(16).toLocaleUpperCase();

  return res;
}

/**
 * 忙等待
 */
function waitReady() {
  /* 根据w25q数据手册，读取状态要先发送命令 0x05 */
  var cmd = [0x05];

  /* 根据w25q数据手册，要读取的数据长度为1，因此定义长度为1的 Unit8Array, 元素值可以是任意值 */
  var val = [0];

  /* 拼接数组为 [0x05, 0] */
  var buf = cmd.concat(val);

  while (1) {
    val = W25Q.readWrite(buf, buf.length);
    if ((val[1] & 0x01) == 0x01) continue;
    else break;
  }
}


/**
 * 擦整个flash
 */
function eraseChip() {
  /* 写使能命令为 0x06 */
  W25Q.write([0x06]);

  /* 芯片擦除命令为 0xc7 */
  W25Q.write([0xc7]);

  /* 等待擦除完成 */
  waitReady();
}

var jedec = getJEDEC();

/* 正确ID为 0x17 */
console.log('ManufacturerID:', jedec.manufacturerId);

/* 正确ID为 0xEF */
console.log('DeviceID:', jedec.deviceId);

eraseChip();

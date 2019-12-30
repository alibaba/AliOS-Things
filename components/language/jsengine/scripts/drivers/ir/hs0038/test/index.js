
var hs0038 = require('./node_modules/hs0038/src/index.js');
var handle = new hs0038('ir');

function code_map(code) {
  if (0xa25d == code) {
    return '1';
  }
  if (0x629d == code) {
    return '2';
  }
  if (0xe21d == code) {
    return '3';
  }

  if (0x22dd == code) {
    return '4';
  }
  if (0x02fd == code) {
    return '5';
  }
  if (0xc23d == code) {
    return '6';
  }
  if (0xe01f == code) {
    return '7';
  }
  if (0xa857 == code) {
    return '8';
  }
  if (0x906f == code) {
    return '9';
  }
  if (0x9867 == code) {
    return '0';
  }
  if (0x6897 == code) {
    return '*';
  }
  if (0xb04f == code) {
    return '#';
  }
  if (0x18e7 == code) {
    return 'up';
  }
  if (0x4ab5 == code) {
    return 'down';
  }
  if (0x38c7 == code) {
    return 'ok';
  }
  if (0x10ef == code) {
    return 'left';
  }
  if (0x5aa5 == code) {
    return 'right';
  }
  return 'error';
}


handle.on(function (data) {
  console.log('code:' + code_map(data));
});
var fs = require('fs');
var path = require('path');
var os = require('os');

function matchObject(item, str) {
  return Object.prototype.toString.call(item)
    === '[object ' + str + ']';
}

function checkStat(name, fn) {
  try {
    return fn(name);
  } catch (err) {
    if (/^(ENOENT|EPERM|EACCES)$/.test(err.code)) {
      if (err.code !== 'ENOENT') {
        console.warn('Warning: Cannot access %s', name);
      }
      return false;
    }
    throw err;
  }
}

var is = {
  nil: function(item) {
    return item == null;
  },
  array: function(item) {
    return Array.isArray(item);
  },
  emptyObject: function(item) {
    for (var key in item) {
      return false;
    }
    return true;
  },
  buffer: function(item) {
    return Buffer.isBuffer(item);
  },
  regExp: function(item) {
    return matchObject(item, 'RegExp');
  },
  string: function(item) {
    return matchObject(item, 'String');
  },
  func: function(item) {
    return typeof item === 'function';
  },
  number: function(item) {
    return matchObject(item, 'Number');
  },
  exists: function(name) {
    return fs.existsSync(name);
  },
  file: function(name) {
    return checkStat(name, function(n) {
      return fs.statSync(n).isFile()
    });
  },
  samePath: function(a, b) {
    return path.resolve(a) === path.resolve(b);
  },
  directory: function(name) {
    return checkStat(name, function(n) {
      return fs.statSync(n).isDirectory()
    });
  },
  symbolicLink: function(name) {
    return checkStat(name, function(n) {
      return fs.lstatSync(n).isSymbolicLink();
    });
  },
  windows: function() {
    return os.platform() === 'win32';
  }
};

module.exports = is;

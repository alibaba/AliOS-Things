"use strict";

function _slicedToArray(arr, i) { return _arrayWithHoles(arr) || _iterableToArrayLimit(arr, i) || _nonIterableRest(); }

function _nonIterableRest() { throw new TypeError("Invalid attempt to destructure non-iterable instance"); }

function _iterableToArrayLimit(arr, i) { var _arr = []; var _n = true; var _d = false; var _e = undefined; try { for (var _i = arr[Symbol.iterator](), _s; !(_n = (_s = _i.next()).done); _n = true) { _arr.push(_s.value); if (i && _arr.length === i) break; } } catch (err) { _d = true; _e = err; } finally { try { if (!_n && _i["return"] != null) _i["return"](); } finally { if (_d) throw _e; } } return _arr; }

function _arrayWithHoles(arr) { if (Array.isArray(arr)) return arr; }

const isPlainObject = require("lodash/isPlainObject"); // the flat plugin map
// This is to prevent dynamic requires - require('babel-plugin-' + name);
// as it suffers during bundling of this code with webpack/browserify
// sorted by option name
// prettier-ignore


const PLUGINS = [// [optionname,         plugin,                                                          default],
["booleans", require("babel-plugin-transform-minify-booleans"), true], ["builtIns", require("babel-plugin-minify-builtins"), true], ["consecutiveAdds", require("babel-plugin-transform-inline-consecutive-adds"), true], ["deadcode", require("babel-plugin-minify-dead-code-elimination"), true], ["evaluate", require("babel-plugin-minify-constant-folding"), true], ["flipComparisons", require("babel-plugin-minify-flip-comparisons"), true], ["guards", require("babel-plugin-minify-guarded-expressions"), true], ["infinity", require("babel-plugin-minify-infinity"), true], ["mangle", require("babel-plugin-minify-mangle-names"), true], ["memberExpressions", require("babel-plugin-transform-member-expression-literals"), true], ["mergeVars", require("babel-plugin-transform-merge-sibling-variables"), true], ["numericLiterals", require("babel-plugin-minify-numeric-literals"), true], ["propertyLiterals", require("babel-plugin-transform-property-literals"), true], ["regexpConstructors", require("babel-plugin-transform-regexp-constructors"), true], ["removeConsole", require("babel-plugin-transform-remove-console"), false], ["removeDebugger", require("babel-plugin-transform-remove-debugger"), false], ["removeUndefined", require("babel-plugin-transform-remove-undefined"), true], ["replace", require("babel-plugin-minify-replace"), true], ["simplify", require("babel-plugin-minify-simplify"), true], ["simplifyComparisons", require("babel-plugin-transform-simplify-comparison-operators"), true], ["typeConstructors", require("babel-plugin-minify-type-constructors"), true], ["undefinedToVoid", require("babel-plugin-transform-undefined-to-void"), true]];
const PROXIES = {
  keepFnName: ["mangle", "deadcode"],
  keepClassName: ["mangle", "deadcode"],
  tdz: ["builtIns", "evaluate", "deadcode", "removeUndefined"]
};
module.exports = preset;

function preset(context, _opts = {}) {
  const opts = isPlainObject(_opts) ? _opts : {}; // validate options

  const validOptions = [...PLUGINS.map(p => p[0]), ...Object.keys(PROXIES)];

  for (let name in opts) {
    if (validOptions.indexOf(name) < 0) {
      throw new Error(`Invalid option "${name}"`);
    }
  } // build a plugins map from the plugin table above


  const pluginsMap = PLUGINS.reduce((acc, [name, plugin, defaultValue]) => Object.assign(acc, {
    [name]: {
      plugin,
      options: null,
      enabled: defaultValue
    }
  }), {}); // handle plugins and their options

  for (var _i = 0; _i < PLUGINS.length; _i++) {
    const _PLUGINS$_i = _slicedToArray(PLUGINS[_i], 1),
          name = _PLUGINS$_i[0];

    if (isPlainObject(opts[name])) {
      // for plugins disabled by default
      pluginsMap[name].enabled = true;
      pluginsMap[name].options = opts[name];
    } else if (opts[name] !== void 0) {
      pluginsMap[name].enabled = !!opts[name];
    }
  } // handle proxies


  for (let proxyname in PROXIES) {
    if (opts[proxyname] !== void 0) {
      var _iteratorNormalCompletion = true;
      var _didIteratorError = false;
      var _iteratorError = undefined;

      try {
        for (var _iterator = PROXIES[proxyname][Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
          const to = _step.value;

          if (!pluginsMap[to].options) {
            pluginsMap[to].options = {};
          }

          if (!hop(pluginsMap[to].options, proxyname)) {
            pluginsMap[to].options[proxyname] = opts[proxyname];
          }
        }
      } catch (err) {
        _didIteratorError = true;
        _iteratorError = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion && _iterator.return != null) {
            _iterator.return();
          }
        } finally {
          if (_didIteratorError) {
            throw _iteratorError;
          }
        }
      }
    }
  } // get the array of plugins


  const plugins = Object.keys(pluginsMap).map(name => pluginsMap[name]).filter(plugin => plugin.enabled).map(plugin => plugin.options ? [plugin.plugin, plugin.options] : plugin.plugin);
  return {
    minified: true,
    presets: [{
      plugins
    }],
    passPerPreset: true
  };
}

function hop(o, key) {
  return Object.prototype.hasOwnProperty.call(o, key);
}
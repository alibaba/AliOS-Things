"use strict";

const VOID_0 = t => t.unaryExpression("void", t.numericLiteral(0), true);

const evaluate = require("babel-helper-evaluate-path"); // Types as Symbols - for comparing types


const types = {}; // This is a test key which is used to avoid Object.keys check
// Object.keys() check is really expensive
// https://gist.github.com/vigneshshanmugam/c766550ecd02292dcdfbf0bf013b9d3d

const testKey = "Expression";

const typeSymbols = t => {
  // don't recompute
  if (types[testKey] !== undefined) {
    return types;
  }

  t.TYPES.forEach(type => {
    types[type] = Symbol.for(type);
  });
  return types;
};

const isNodeOfType = (t, node, typeSymbol) => typeof typeSymbol !== "symbol" ? false : t["is" + Symbol.keyFor(typeSymbol)](node);

const isPatternMatchesPath = t => function _isPatternMatchesPath(patternValue, inputPath) {
  if (Array.isArray(patternValue)) {
    for (let i = 0; i < patternValue.length; i++) {
      if (_isPatternMatchesPath(patternValue[i], inputPath)) {
        return true;
      }
    }

    return false;
  }

  if (typeof patternValue === "function") {
    return patternValue(inputPath);
  }

  if (isNodeOfType(t, inputPath.node, patternValue)) return true;
  const evalResult = evaluate(inputPath);
  if (!evalResult.confident || !inputPath.isPure()) return false;
  return evalResult.value === patternValue;
};

module.exports = {
  VOID_0,
  // Types as Symbols
  typeSymbols,
  // This is required for resolving type aliases
  isNodeOfType,
  isPatternMatchesPath
};
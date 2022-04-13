"use strict";

const h = require("./helpers");

const PatternMatch = require("./pattern-match");

const evaluate = require("babel-helper-evaluate-path");

module.exports = t => {
  const OP_AND = input => input === "&&";

  const OP_OR = input => input === "||";

  function simplifyPatterns(path) {
    // cache of evaluate(path)
    const evaluateMemo = new Map();

    const TRUTHY = input => {
      // !NaN and !undefined are truthy
      // separate check here as they are considered impure by babel
      if (input.isUnaryExpression() && input.get("argument").isIdentifier()) {
        if (input.node.argument.name === "NaN" || input.node.argument.name === "undefined") {
          return true;
        }
      }

      const evalResult = evaluate(input);
      evaluateMemo.set(input, evalResult);
      return evalResult.confident && input.isPure() && evalResult.value;
    };

    const FALSY = input => {
      // NaN and undefined are falsy
      // separate check here as they are considered impure by babel
      if (input.isIdentifier()) {
        if (input.node.name === "NaN" || input.node.name === "undefined") {
          return true;
        }
      }

      const evalResult = evaluate(input);
      evaluateMemo.set(input, evalResult);
      return evalResult.confident && input.isPure() && !evalResult.value;
    };

    const _h$typeSymbols = h.typeSymbols(t),
          EX = _h$typeSymbols.Expression; // Convention:
    // [left, operator, right, handler(leftNode, rightNode)]


    const matcher = new PatternMatch([[TRUTHY, OP_AND, EX, (l, r) => r], [FALSY, OP_AND, EX, l => l], [TRUTHY, OP_OR, EX, l => l], [FALSY, OP_OR, EX, (l, r) => r]]);
    const left = path.get("left");
    const right = path.get("right");
    const operator = path.node.operator;
    const result = matcher.match([left, operator, right], h.isPatternMatchesPath(t));

    if (result.match) {
      // here we are sure that left.evaluate is always confident becuase
      // it satisfied one of TRUTHY/FALSY paths
      let value;

      if (evaluateMemo.has(left)) {
        value = evaluateMemo.get(left).value;
      } else {
        value = evaluate(left).value;
      }

      path.replaceWith(result.value(t.valueToNode(value), right.node));
    }
  }

  return {
    simplifyPatterns
  };
};
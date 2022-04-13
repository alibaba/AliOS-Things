"use strict";

const h = require("./helpers");

const PatternMatch = require("./pattern-match");

module.exports = t => {
  // small abstractions
  const not = node => t.unaryExpression("!", node);

  const notnot = node => not(not(node));

  const or = (a, b) => t.logicalExpression("||", a, b);

  const and = (a, b) => t.logicalExpression("&&", a, b);

  function simplifyPatterns(path) {
    const test = path.get("test");
    const consequent = path.get("consequent");
    const alternate = path.get("alternate");

    const _h$typeSymbols = h.typeSymbols(t),
          EX = _h$typeSymbols.Expression; // Convention:
    // ===============
    // for each pattern [test, consequent, alternate, handler(expr, cons, alt)]


    const matcher = new PatternMatch([[EX, true, false, e => notnot(e)], [EX, false, true, e => not(e)], [EX, true, EX, (e, c, a) => or(notnot(e), a)], [EX, false, EX, (e, c, a) => and(not(e), a)], [EX, EX, true, (e, c) => or(not(e), c)], [EX, EX, false, (e, c) => and(notnot(e), c)]]);
    const result = matcher.match([test, consequent, alternate], h.isPatternMatchesPath(t));

    if (result.match) {
      path.replaceWith(result.value(test.node, consequent.node, alternate.node));
    }
  }

  return {
    simplifyPatterns
  };
};
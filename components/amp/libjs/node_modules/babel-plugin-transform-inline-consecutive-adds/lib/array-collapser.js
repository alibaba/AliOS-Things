"use strict";

const Collapser = require("./collapser");

class ArrayCollapser extends Collapser {
  isInitTypeValid(init) {
    return init.isArrayExpression();
  }

  isExpressionTypeValid(expr) {
    return expr.isCallExpression();
  }

  getExpressionChecker(objName, checkReference) {
    return expr => {
      // checks expr is of form:
      // foo.push(rval1, ...nrvals)
      const callee = expr.get("callee");

      if (!callee.isMemberExpression()) {
        return false;
      }

      const obj = callee.get("object"),
            prop = callee.get("property");

      if (!obj.isIdentifier() || obj.node.name !== objName || !prop.isIdentifier() || prop.node.name !== "push") {
        return false;
      }

      const args = expr.get("arguments");

      if (args.some(checkReference)) {
        return false;
      }

      return true;
    };
  }

  extractAssignment(expr) {
    return expr.node.arguments;
  }

  addSuccessfully(t, args, init) {
    args.map(a => init.elements.push(a));
    return true;
  }

}

module.exports = ArrayCollapser;
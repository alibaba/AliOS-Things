"use strict";

const Collapser = require("./collapser");

class SetCollapser extends Collapser {
  isInitTypeValid(init) {
    return init.isNewExpression() && init.get("callee").isIdentifier() && init.node.callee.name === "Set" && ( // other iterables might not be append-able
    init.node.arguments.length === 0 || init.node.arguments.length === 1 && init.get("arguments")[0].isArrayExpression());
  }

  isExpressionTypeValid(expr) {
    return expr.isCallExpression();
  }

  getExpressionChecker(objName, checkReference) {
    return expr => {
      // checks expr is of form:
      // foo.add(rval)
      const callee = expr.get("callee");

      if (!callee.isMemberExpression()) {
        return false;
      }

      const obj = callee.get("object"),
            prop = callee.get("property");

      if (!obj.isIdentifier() || obj.node.name !== objName || !prop.isIdentifier() || prop.node.name !== "add") {
        return false;
      }

      const args = expr.get("arguments");

      if (args.length !== 1) {
        return false;
      }

      if (checkReference(args[0])) {
        return false;
      }

      return true;
    };
  }

  extractAssignment(expr) {
    return expr.node.arguments[0];
  }

  addSuccessfully(t, arg, init) {
    if (init.arguments.length === 0) {
      init.arguments.push(t.arrayExpression());
    }

    init.arguments[0].elements.push(arg);
    return true;
  }

}

module.exports = SetCollapser;
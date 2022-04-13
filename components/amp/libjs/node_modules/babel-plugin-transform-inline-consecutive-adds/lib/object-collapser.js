"use strict";

const Collapser = require("./collapser");

class ObjectCollapser extends Collapser {
  isInitTypeValid(init) {
    return init.isObjectExpression();
  }

  isExpressionTypeValid(expr) {
    return expr.isAssignmentExpression();
  }

  getExpressionChecker(objName, checkReference) {
    return expr => {
      // checks expr is of form:
      // foo.a = rval | foo[a] = rval
      const left = expr.get("left");

      if (!left.isMemberExpression()) {
        return false;
      }

      const obj = left.get("object"),
            prop = left.get("property");

      if (!obj.isIdentifier() || obj.node.name !== objName) {
        return false;
      }

      if (!prop.isIdentifier() && checkReference(prop)) {
        return false;
      }

      if (left.node.computed && !(prop.isStringLiteral() || prop.isNumericLiteral())) {
        return false;
      }

      const right = expr.get("right");

      if (checkReference(right)) {
        return false;
      }

      return true;
    };
  }

  extractAssignment(expr) {
    return [expr.node.left.property, expr.node.right];
  }

  addSuccessfully(t, [left, right], init) {
    init.properties.push(t.objectProperty(left, right));
    return true;
  }

}

module.exports = ObjectCollapser;
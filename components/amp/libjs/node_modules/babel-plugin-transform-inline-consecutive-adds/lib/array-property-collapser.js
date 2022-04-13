"use strict";

const Collapser = require("./collapser");

class ArrayPropertyCollapser extends Collapser {
  isInitTypeValid(init) {
    return init.isArrayExpression();
  }

  isExpressionTypeValid(expr) {
    return expr.isAssignmentExpression();
  }

  getExpressionChecker(objName, checkReference) {
    return expr => {
      // checks expr is of form:
      // foo[num] = rval
      const left = expr.get("left");

      if (!left.isMemberExpression()) {
        return false;
      }

      const obj = left.get("object"),
            prop = left.get("property");

      if (!obj.isIdentifier() || obj.node.name !== objName) {
        return false;
      }

      const checkIndex = num => Number.isInteger(num) && num >= 0;

      if (!(prop.isNumericLiteral() || prop.isStringLiteral()) || !checkIndex(Number(prop.node.value))) {
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
    return [expr.node.left.property.value, expr.get("right")];
  }

  addSuccessfully(t, [index, rval], init) {
    const elements = init.elements;

    for (let i = elements.length; i <= index; i++) {
      elements.push(null);
    }

    if (elements[index] !== null) {
      return false;
    }

    elements[index] = rval.node;
    return true;
  }

  isSizeSmaller({
    newInit,
    oldInit,
    varDecl,
    assignments,
    statements
  }) {
    const anyUndefined = args => args.some(a => a === undefined); // We make an inexact calculation of how much space we save.
    // It's inexact because we don't know how whitespaces will get minimized,
    // and other factors.


    if (anyUndefined([statements[statements.length - 1].node.end, varDecl.node.end])) {
      return false;
    }

    const statementsLength = statements[statements.length - 1].node.end - varDecl.node.end; // Approx. formula of the change in `init`'s length =
    // (# commas added) + (size of all the new rvals added), where
    // # commas added = (difference between the lengths of the old and new arrays)

    const numCommaAdded = newInit.elements.length - oldInit.elements.length;

    if (anyUndefined(assignments.map(([, rval]) => rval.node.end)) || anyUndefined(assignments.map(([, rval]) => rval.node.start))) {
      return false;
    }

    const sizeOfRvals = assignments.map(([, rval]) => rval.node.end - rval.node.start + 1).reduce((a, b) => a + b, 0); // add 1 for space in front // sum

    return numCommaAdded + sizeOfRvals < statementsLength;
  }

}

module.exports = ArrayPropertyCollapser;
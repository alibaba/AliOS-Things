"use strict";

const operators = new Set(["+", "-", "*", "%", "<<", ">>", ">>>", "&", "|", "^", "/", "**"]);
const updateOperators = new Set(["+", "-"]);

module.exports = t => {
  function simplify(path) {
    const rightExpr = path.get("right");
    const leftExpr = path.get("left");

    if (path.node.operator !== "=") {
      return;
    }

    const canBeUpdateExpression = rightExpr.get("right").isNumericLiteral() && rightExpr.get("right").node.value === 1 && updateOperators.has(rightExpr.node.operator);

    if (leftExpr.isMemberExpression()) {
      const leftPropNames = getPropNames(leftExpr);
      const rightPropNames = getPropNames(rightExpr.get("left"));

      if (!leftPropNames || leftPropNames.indexOf(undefined) > -1 || !rightPropNames || rightPropNames.indexOf(undefined) > -1 || !operators.has(rightExpr.node.operator) || !areArraysEqual(leftPropNames, rightPropNames)) {
        return;
      }
    } else {
      if (!rightExpr.isBinaryExpression() || !operators.has(rightExpr.node.operator) || leftExpr.node.name !== rightExpr.node.left.name) {
        return;
      }
    }

    let newExpression; // special case x=x+1 --> ++x

    if (canBeUpdateExpression) {
      newExpression = t.updateExpression(rightExpr.node.operator + rightExpr.node.operator, t.clone(leftExpr.node), true
      /* prefix */
      );
    } else {
      newExpression = t.assignmentExpression(rightExpr.node.operator + "=", t.clone(leftExpr.node), t.clone(rightExpr.node.right));
    }

    path.replaceWith(newExpression);
  }

  return {
    simplify
  };
};

function areArraysEqual(arr1, arr2) {
  return arr1.every((value, index) => {
    return String(value) === String(arr2[index]);
  });
}

function getPropNames(path) {
  if (!path.isMemberExpression()) {
    return;
  }

  let obj = path.get("object");
  const prop = path.get("property");
  const propNames = [getName(prop.node)];

  while (obj.type === "MemberExpression") {
    const node = obj.get("property").node;

    if (node) {
      propNames.push(getName(node));
    }

    obj = obj.get("object");
  }

  propNames.push(getName(obj.node));
  return propNames;
}

function getName(node) {
  if (node.type === "ThisExpression") {
    return "this";
  }

  if (node.type === "Super") {
    return "super";
  }

  if (node.type === "NullLiteral") {
    return "null";
  } // augment identifiers so that they don't match
  // string/number literals
  // but still match against each other


  return node.name ? node.name + "_" : node.value
  /* Literal */
  ;
}
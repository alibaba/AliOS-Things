"use strict";

const evaluate = require("babel-helper-evaluate-path");

const _require = require("./replacements"),
      FALLBACK_HANDLER = _require.FALLBACK_HANDLER;

function getName(member) {
  if (member.computed) {
    switch (member.property.type) {
      case "StringLiteral":
      case "NumericLiteral":
        return member.property.value;

      case "TemplateLiteral":
        return;
    }
  } else {
    return member.property.name;
  }
}

function swap(path, member, handlers, ...args) {
  const key = getName(member.node);
  if (key === void 0) return false;
  let handler;

  if (hop(handlers, key) && typeof handlers[key] === "function") {
    handler = handlers[key];
  } else if (typeof handlers[FALLBACK_HANDLER] === "function") {
    handler = handlers[FALLBACK_HANDLER].bind(member.get("object"), key);
  } else {
    return false;
  }

  const replacement = handler.apply(member.get("object"), args);

  if (replacement) {
    path.replaceWith(replacement);
    return true;
  }

  return false;
}

module.exports = babel => {
  const replacements = require("./replacements.js")(babel);

  const seen = Symbol("seen");
  const t = babel.types;
  return {
    name: "minify-constant-folding",
    visitor: {
      // Evaluate string expressions that are next to each other
      // but are not actually a binary expression.
      // "a" + b + "c" + "d" -> "a" + b + "cd"
      BinaryExpression(path) {
        if (!path.isBinaryExpression({
          operator: "+"
        })) {
          return;
        }

        let literal, bin;
        const left = path.get("left");
        const right = path.get("right");

        if (right.isStringLiteral()) {
          literal = right;

          if (left.isBinaryExpression({
            operator: "+"
          })) {
            bin = left;
          } else {
            return;
          }
        } else if (left.isStringLiteral()) {
          literal = left;

          if (right.isBinaryExpression({
            operator: "+"
          })) {
            bin = right;
          } else {
            return;
          }
        } else {
          return;
        }

        const relevant = getLeaf(bin, literal.key);

        if (!relevant) {
          return;
        }

        const value = literal.key === "right" ? relevant.node.value + literal.node.value : literal.node.value + relevant.node.value;
        relevant.replaceWith(t.stringLiteral(value));
        path.replaceWith(bin.node);

        function getLeaf(path, direction) {
          if (path.isStringLiteral()) {
            return path;
          } else if (path.isBinaryExpression({
            operator: "+"
          })) {
            return getLeaf(path.get(direction), direction);
          }
        }
      },

      // TODO: look into evaluating binding too (could result in more code, but gzip?)
      Expression(path, {
        opts: {
          tdz = false
        } = {}
      }) {
        const node = path.node,
              parent = path.parent;

        if (node[seen]) {
          return;
        }

        if (path.isLiteral()) {
          return;
        }

        if (!path.isPure()) {
          return;
        } // Avoid replacing the values for identifiers in exports


        if (t.isExportSpecifier(parent)) {
          return;
        } // -0 maybe compared via dividing and then checking against -Infinity
        // Also -X will always be -X.


        if (t.isUnaryExpression(node, {
          operator: "-"
        }) && t.isNumericLiteral(node.argument)) {
          return;
        } // We have a transform that converts true/false to !0/!1


        if (t.isUnaryExpression(node, {
          operator: "!"
        }) && t.isNumericLiteral(node.argument)) {
          if (node.argument.value === 0 || node.argument.value === 1) {
            return;
          }
        } // void 0 is used for undefined.


        if (t.isUnaryExpression(node, {
          operator: "void"
        }) && t.isNumericLiteral(node.argument, {
          value: 0
        })) {
          return;
        }

        const res = evaluate(path, {
          tdz
        });

        if (res.confident) {
          // Avoid fractions because they can be longer than the original expression.
          // There is also issues with number precision?
          if (typeof res.value === "number" && !Number.isInteger(res.value)) {
            return;
          } // Preserve -0


          if (typeof res.value === "number" && res.value === 0) {
            if (1 / res.value === -Infinity) {
              const node = t.unaryExpression("-", t.numericLiteral(0), true);
              node[seen] = true;
              path.replaceWith(node);
              return;
            }
          } // this will convert object to object but
          // t.valueToNode has other effects where property name
          // is not treated for the respective environment.
          // So we bail here for objects and let other plugins
          // take care of converting String literal to Identifier


          if (typeof res.value === "object") {
            return;
          }

          const node = t.valueToNode(res.value);
          node[seen] = true;
          path.replaceWith(node);
        }
      },

      CallExpression(path) {
        const node = path.node;
        const member = path.get("callee");

        if (t.isMemberExpression(member)) {
          const helpers = replacements[member.node.object.type];
          if (!helpers || !helpers.calls) return; // find if the input can be constant folded

          if (typeof helpers.canReplace === "function" && !helpers.canReplace.call(member.get("object"))) {
            return;
          }

          swap(path, member, helpers.calls, ...node.arguments);
        }
      },

      MemberExpression(path) {
        const node = path.node;
        const helpers = replacements[node.object.type];
        if (!helpers || !helpers.members) return;
        swap(path, path, helpers.members);
      }

    }
  };
};

function hop(o, key) {
  return Object.prototype.hasOwnProperty.call(o, key);
}
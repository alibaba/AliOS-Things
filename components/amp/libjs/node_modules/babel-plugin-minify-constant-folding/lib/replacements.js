"use strict";

const FALLBACK_HANDLER = Symbol("fallback handler");

module.exports = ({
  types: t
}) => {
  const undef = t.unaryExpression("void", t.numericLiteral(0));

  function isUndef(ob) {
    return ob === undefined || t.isIdentifier(ob, {
      name: "undefined"
    }) || t.isUnaryExpression(ob, {
      operator: "void"
    });
  }

  function defaultZero(cb) {
    return function (i = t.numericLiteral(0), ...args) {
      if (t.isNumericLiteral(i)) {
        return cb.call(this.node, this.node, i.value, ...args);
      }
    };
  }

  function hasSpread(node) {
    return node.elements.some(el => t.isSpreadElement(el));
  }

  return {
    ArrayExpression: {
      canReplace() {
        return !hasSpread(this.node);
      },

      members: {
        length() {
          if (hasSpread(this.node)) {
            return;
          }

          return t.numericLiteral(this.node.elements.length);
        },

        [FALLBACK_HANDLER](i) {
          if (hasSpread(this.node)) {
            return;
          }

          if (typeof i === "number" || i.match(/^\d+$/)) {
            return this.node.elements[i] || undef;
          }
        }

      },
      calls: {
        join(sep = t.stringLiteral(",")) {
          if (!t.isStringLiteral(sep)) return;
          let bad = false;
          const str = this.get("elements").map(el => {
            const evaled = el.evaluate();

            if (!evaled.confident) {
              bad = true;
              return;
            }

            return evaled.value;
          }).join(sep.value);
          return bad ? void 0 : t.stringLiteral(str);
        },

        push(...args) {
          return t.numericLiteral(this.node.elements.length + args.length);
        },

        shift() {
          if (this.node.elements.length === 0) {
            return undef;
          }

          return t.numericLiteral(this.node.elements.length - 1);
        },

        slice(start = t.numericLiteral(0), end) {
          if (!t.isNumericLiteral(start) || end && !t.isNumericLiteral(end)) {
            return;
          }

          return t.arrayExpression(this.node.elements.slice(start.value, end && end.value));
        },

        pop() {
          return this.node.elements[this.node.elements.length - 1] || undef;
        },

        reverse() {
          return t.arrayExpression(this.node.elements.reverse());
        },

        splice(start, end, ...args) {
          if (!t.isNumericLiteral(start) || end && !t.isNumericLiteral(end)) {
            return;
          }

          if (end) {
            args.unshift(end.value);
          }

          return t.arrayExpression(this.node.elements.slice().splice(start.value, ...args));
        }

      }
    },
    StringLiteral: {
      members: {
        length() {
          return t.numericLiteral(this.node.value.length);
        },

        [FALLBACK_HANDLER](i) {
          if (typeof i === "number" || i.match(/^\d+$/)) {
            const ch = this.node.value[i];
            return ch ? t.stringLiteral(ch) : undef;
          }
        }

      },
      calls: {
        split(sep = undef) {
          let realSep = null;

          if (t.isStringLiteral(sep)) {
            realSep = sep.value;
          }

          if (isUndef(sep)) {
            realSep = sep;
          }

          if (realSep !== null) {
            return t.arrayExpression(this.node.value.split(realSep).map(str => t.stringLiteral(str)));
          }
        },

        charAt: defaultZero(({
          value
        }, i) => t.stringLiteral(value.charAt(i))),
        charCodeAt: defaultZero(({
          value
        }, i) => t.numericLiteral(value.charCodeAt(i))),
        codePointAt: defaultZero(({
          value
        }, i) => t.numericLiteral(value.codePointAt(i)))
      }
    }
  };
};

module.exports.FALLBACK_HANDLER = FALLBACK_HANDLER;
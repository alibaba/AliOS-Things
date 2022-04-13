"use strict";

const evaluate = require("babel-helper-evaluate-path");

function isPureAndUndefined(rval, {
  tdz,
  scope = {
    hasBinding: () => false
  }
} = {}) {
  if (rval.isIdentifier() && rval.node.name === "undefined") {
    // deopt right away if undefined is a local binding
    if (scope.hasBinding(rval.node.name, true
    /* no globals */
    )) {
      return false;
    }

    return true;
  }

  if (!rval.isPure()) {
    return false;
  }

  const evaluation = evaluate(rval, {
    tdz
  });
  return evaluation.confident === true && evaluation.value === undefined;
}

function getLoopParent(path, scopeParent) {
  const parent = path.findParent(p => p.isLoop() || p === scopeParent); // don't traverse higher than the function the var is defined in.

  return parent === scopeParent ? null : parent;
}

function getFunctionParent(path, scopeParent) {
  const parent = path.findParent(p => p.isFunction()); // don't traverse higher than the function the var is defined in.

  return parent === scopeParent ? null : parent;
}

function getFunctionReferences(path, scopeParent, references = new Set()) {
  for (let func = getFunctionParent(path, scopeParent); func; func = getFunctionParent(func, scopeParent)) {
    const id = func.node.id;
    const binding = id && func.scope.getBinding(id.name);

    if (!binding) {
      continue;
    }

    binding.referencePaths.forEach(path => {
      if (!references.has(path)) {
        references.add(path);
        getFunctionReferences(path, scopeParent, references);
      }
    });
  }

  return references;
}

function hasViolation(declarator, scope, start) {
  const binding = scope.getBinding(declarator.node.id.name);

  if (!binding) {
    return true;
  }

  const scopeParent = declarator.getFunctionParent();
  const violation = binding.constantViolations.some(v => {
    // https://github.com/babel/minify/issues/630
    if (!v.node) {
      return false;
    } // return 'true' if we cannot guarantee the violation references
    // the initialized identifier after


    const violationStart = v.node.start;

    if (violationStart === undefined || violationStart < start) {
      return true;
    }

    const references = getFunctionReferences(v, scopeParent);
    var _iteratorNormalCompletion = true;
    var _didIteratorError = false;
    var _iteratorError = undefined;

    try {
      for (var _iterator = references[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
        const ref = _step.value;

        if (ref.node.start === undefined || ref.node.start < start) {
          return true;
        }
      }
    } catch (err) {
      _didIteratorError = true;
      _iteratorError = err;
    } finally {
      try {
        if (!_iteratorNormalCompletion && _iterator.return != null) {
          _iterator.return();
        }
      } finally {
        if (_didIteratorError) {
          throw _iteratorError;
        }
      }
    }

    for (let loop = getLoopParent(declarator, scopeParent); loop; loop = getLoopParent(loop, scopeParent)) {
      if (loop.node.end === undefined || loop.node.end > violationStart) {
        return true;
      }
    }
  });
  return violation;
}

module.exports = function () {
  return {
    name: "transform-remove-undefined",
    visitor: {
      SequenceExpression(path, {
        opts: {
          tdz
        } = {}
      }) {
        const expressions = path.get("expressions");

        for (let i = 0; i < expressions.length; i++) {
          const expr = expressions[i];
          if (!isPureAndUndefined(expr, {
            tdz,
            scope: path.scope
          })) continue; // last value

          if (i === expressions.length - 1) {
            if (path.parentPath.isExpressionStatement()) {
              expr.remove();
            }
          } else {
            expr.remove();
          }
        }
      },

      ReturnStatement(path, {
        opts: {
          tdz
        } = {}
      }) {
        if (path.node.argument !== null) {
          if (isPureAndUndefined(path.get("argument"), {
            tdz,
            scope: path.scope
          })) {
            path.node.argument = null;
          }
        }
      },

      VariableDeclaration(path, {
        opts: {
          tdz
        } = {}
      }) {
        switch (path.node.kind) {
          case "const":
            break;

          case "let":
            var _iteratorNormalCompletion2 = true;
            var _didIteratorError2 = false;
            var _iteratorError2 = undefined;

            try {
              for (var _iterator2 = path.get("declarations")[Symbol.iterator](), _step2; !(_iteratorNormalCompletion2 = (_step2 = _iterator2.next()).done); _iteratorNormalCompletion2 = true) {
                const declarator = _step2.value;

                if (isPureAndUndefined(declarator.get("init"), {
                  tdz
                })) {
                  declarator.node.init = null;
                }
              }
            } catch (err) {
              _didIteratorError2 = true;
              _iteratorError2 = err;
            } finally {
              try {
                if (!_iteratorNormalCompletion2 && _iterator2.return != null) {
                  _iterator2.return();
                }
              } finally {
                if (_didIteratorError2) {
                  throw _iteratorError2;
                }
              }
            }

            break;

          case "var":
            const start = path.node.start;

            if (start === undefined) {
              // This is common for plugin-generated nodes
              break;
            }

            const scope = path.scope;
            var _iteratorNormalCompletion3 = true;
            var _didIteratorError3 = false;
            var _iteratorError3 = undefined;

            try {
              for (var _iterator3 = path.get("declarations")[Symbol.iterator](), _step3; !(_iteratorNormalCompletion3 = (_step3 = _iterator3.next()).done); _iteratorNormalCompletion3 = true) {
                const declarator = _step3.value;

                if (isPureAndUndefined(declarator.get("init")) && !hasViolation(declarator, scope, start)) {
                  declarator.node.init = null;
                }
              }
            } catch (err) {
              _didIteratorError3 = true;
              _iteratorError3 = err;
            } finally {
              try {
                if (!_iteratorNormalCompletion3 && _iterator3.return != null) {
                  _iterator3.return();
                }
              } finally {
                if (_didIteratorError3) {
                  throw _iteratorError3;
                }
              }
            }

            break;
        }
      }

    }
  };
};
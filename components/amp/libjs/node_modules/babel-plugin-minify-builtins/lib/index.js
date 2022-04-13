"use strict"; // Assuming all the static methods from below array are side effect free evaluation
// except Math.random

function _slicedToArray(arr, i) { return _arrayWithHoles(arr) || _iterableToArrayLimit(arr, i) || _nonIterableRest(); }

function _nonIterableRest() { throw new TypeError("Invalid attempt to destructure non-iterable instance"); }

function _iterableToArrayLimit(arr, i) { var _arr = []; var _n = true; var _d = false; var _e = undefined; try { for (var _i = arr[Symbol.iterator](), _s; !(_n = (_s = _i.next()).done); _n = true) { _arr.push(_s.value); if (i && _arr.length === i) break; } } catch (err) { _d = true; _e = err; } finally { try { if (!_n && _i["return"] != null) _i["return"](); } finally { if (_d) throw _e; } } return _arr; }

function _arrayWithHoles(arr) { if (Array.isArray(arr)) return arr; }

const VALID_CALLEES = ["String", "Number", "Math"];
const INVALID_METHODS = ["random"];
const newIssueUrl = "https://github.com/babel/minify/issues/new";

module.exports = function ({
  types: t
}) {
  class BuiltInReplacer {
    constructor() {
      // map<expr_name, path[]>;
      this.pathsToUpdate = new Map();
    }

    getCollectVisitor() {
      const context = this;
      const collectVisitor = {
        AssignmentExpression(path) {
          const left = path.node.left; // Should bail and not run the plugin
          // when builtin is polyfilled

          if (t.isMemberExpression(left) && isBuiltInComputed(left)) {
            let parent = path;

            do {
              parent.stop();
            } while (parent = parent.parentPath);
          }
        },

        MemberExpression(path) {
          if (path.parentPath.isCallExpression()) {
            return;
          }

          const node = path.node;

          if (!isComputed(node) && isBuiltin(node) && !getFunctionParent(path).isProgram()) {
            const expName = memberToString(node);
            addToMap(context.pathsToUpdate, expName, path);
          }
        },

        CallExpression: {
          exit(path) {
            const callee = path.get("callee");

            if (!callee.isMemberExpression()) {
              return;
            }

            const node = callee.node; // computed property should not be optimized
            // Math[max]() -> Math.max()

            if (!isComputed(node) && isBuiltin(node) && !getFunctionParent(callee).isProgram()) {
              const expName = memberToString(node);
              addToMap(context.pathsToUpdate, expName, callee);
            }
          }

        }
      };
      return collectVisitor;
    }

    replace() {
      var _iteratorNormalCompletion = true;
      var _didIteratorError = false;
      var _iteratorError = undefined;

      try {
        for (var _iterator = this.pathsToUpdate[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
          const _step$value = _slicedToArray(_step.value, 2),
                expName = _step$value[0],
                paths = _step$value[1];

          // transform only if there is more than 1 occurence
          if (paths.length <= 1) {
            continue;
          }

          const segmentsMap = getSegmentedSubPaths(paths);
          var _iteratorNormalCompletion2 = true;
          var _didIteratorError2 = false;
          var _iteratorError2 = undefined;

          try {
            for (var _iterator2 = segmentsMap[Symbol.iterator](), _step2; !(_iteratorNormalCompletion2 = (_step2 = _iterator2.next()).done); _iteratorNormalCompletion2 = true) {
              const _step2$value = _slicedToArray(_step2.value, 2),
                    parent = _step2$value[0],
                    subpaths = _step2$value[1];

              if (subpaths.length <= 1) {
                continue;
              }

              const uniqueIdentifier = parent.scope.generateUidIdentifier(expName);
              const newNode = t.variableDeclaration("var", [t.variableDeclarator(uniqueIdentifier, subpaths[0].node)]);
              var _iteratorNormalCompletion3 = true;
              var _didIteratorError3 = false;
              var _iteratorError3 = undefined;

              try {
                for (var _iterator3 = subpaths[Symbol.iterator](), _step3; !(_iteratorNormalCompletion3 = (_step3 = _iterator3.next()).done); _iteratorNormalCompletion3 = true) {
                  const path = _step3.value;
                  path.replaceWith(t.clone(uniqueIdentifier));
                } // hoist the created var to the top of the function scope

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

              const target = parent.get("body");
              /**
               * Here, we validate a case where there is a local binding of
               * one of Math, String or Number. Here we have to get the
               * global Math instead of using the local one - so we do the
               * following transformation
               *
               * var _Mathmax = Math.max;
               *
               * to
               *
               * var _Mathmax = (0, eval)("this").Math.max;
               */

              for (var _i2 = 0; _i2 < VALID_CALLEES.length; _i2++) {
                const builtin = VALID_CALLEES[_i2];

                if (target.scope.getBinding(builtin)) {
                  const prev = newNode.declarations[0].init;

                  if (!t.isMemberExpression(prev)) {
                    throw new Error(`minify-builtins expected a MemberExpression. ` + `Found ${prev.type}. ` + `Please report this at ${newIssueUrl}`);
                  }

                  if (!t.isMemberExpression(prev.object)) {
                    newNode.declarations[0].init = t.memberExpression(t.memberExpression(getGlobalThis(), prev.object), prev.property);
                  }
                }
              }

              target.unshiftContainer("body", newNode);
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
    }

  }

  const builtInReplacer = new BuiltInReplacer();
  return {
    name: "minify-builtins",
    visitor: Object.assign({}, builtInReplacer.getCollectVisitor(), {
      Program: {
        exit() {
          builtInReplacer.replace();
        }

      }
    })
  };

  function memberToString(memberExprNode) {
    const object = memberExprNode.object,
          property = memberExprNode.property;
    let result = "";
    if (t.isIdentifier(object)) result += object.name;
    if (t.isMemberExpression(object)) result += memberToString(object);
    if (t.isIdentifier(property)) result += property.name;
    return result;
  }

  function isBuiltInComputed(memberExprNode) {
    const object = memberExprNode.object,
          computed = memberExprNode.computed;
    return computed && t.isIdentifier(object) && VALID_CALLEES.indexOf(object.name) >= 0;
  }

  function isBuiltin(memberExprNode) {
    const object = memberExprNode.object,
          property = memberExprNode.property;

    if (t.isIdentifier(object) && t.isIdentifier(property) && VALID_CALLEES.indexOf(object.name) >= 0 && INVALID_METHODS.indexOf(property.name) < 0) {
      return true;
    }

    return false;
  } // Creates a segmented map that contains the earliest common Ancestor
  // as the key and array of subpaths that are descendats of the LCA as value


  function getSegmentedSubPaths(paths) {
    let segments = new Map(); // Get earliest Path in tree where paths intersect

    paths[0].getDeepestCommonAncestorFrom(paths, (lastCommon, index, ancestries) => {
      // found the LCA
      if (!lastCommon.isProgram()) {
        let fnParent;

        if (lastCommon.isFunction() && t.isBlockStatement(lastCommon.node.body)) {
          segments.set(lastCommon, paths);
          return;
        } else if (!(fnParent = getFunctionParent(lastCommon)).isProgram() && t.isBlockStatement(fnParent.node.body)) {
          segments.set(fnParent, paths);
          return;
        }
      } // Deopt and construct segments otherwise


      var _iteratorNormalCompletion4 = true;
      var _didIteratorError4 = false;
      var _iteratorError4 = undefined;

      try {
        for (var _iterator4 = ancestries[Symbol.iterator](), _step4; !(_iteratorNormalCompletion4 = (_step4 = _iterator4.next()).done); _iteratorNormalCompletion4 = true) {
          const ancestor = _step4.value;
          const fnPath = getChildFuncion(ancestor);

          if (fnPath === void 0) {
            continue;
          }

          const validDescendants = paths.filter(p => {
            return p.isDescendant(fnPath);
          });
          segments.set(fnPath, validDescendants);
        }
      } catch (err) {
        _didIteratorError4 = true;
        _iteratorError4 = err;
      } finally {
        try {
          if (!_iteratorNormalCompletion4 && _iterator4.return != null) {
            _iterator4.return();
          }
        } finally {
          if (_didIteratorError4) {
            throw _iteratorError4;
          }
        }
      }
    });
    return segments;
  }

  function getChildFuncion(ancestors = []) {
    var _iteratorNormalCompletion5 = true;
    var _didIteratorError5 = false;
    var _iteratorError5 = undefined;

    try {
      for (var _iterator5 = ancestors[Symbol.iterator](), _step5; !(_iteratorNormalCompletion5 = (_step5 = _iterator5.next()).done); _iteratorNormalCompletion5 = true) {
        const path = _step5.value;

        if (path.isFunction() && t.isBlockStatement(path.node.body)) {
          return path;
        }
      }
    } catch (err) {
      _didIteratorError5 = true;
      _iteratorError5 = err;
    } finally {
      try {
        if (!_iteratorNormalCompletion5 && _iterator5.return != null) {
          _iterator5.return();
        }
      } finally {
        if (_didIteratorError5) {
          throw _iteratorError5;
        }
      }
    }
  }
  /**
   * returns
   *
   * (0, eval)("this")
   */


  function getGlobalThis() {
    return t.callExpression(t.sequenceExpression([t.valueToNode(0), t.identifier("eval")]), [t.valueToNode("this")]);
  }
};

function addToMap(map, key, value) {
  if (!map.has(key)) {
    map.set(key, []);
  }

  map.get(key).push(value);
}

function isComputed(node) {
  return node.computed;
}
/**
 * Babel-7 returns null if there is no function parent
 * and uses getProgramParent to get Program
 */


function getFunctionParent(path) {
  return (path.scope.getFunctionParent() || path.scope.getProgramParent()).path;
}
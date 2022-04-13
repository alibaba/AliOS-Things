"use strict";

function _slicedToArray(arr, i) { return _arrayWithHoles(arr) || _iterableToArrayLimit(arr, i) || _nonIterableRest(); }

function _nonIterableRest() { throw new TypeError("Invalid attempt to destructure non-iterable instance"); }

function _iterableToArrayLimit(arr, i) { var _arr = []; var _n = true; var _d = false; var _e = undefined; try { for (var _i = arr[Symbol.iterator](), _s; !(_n = (_s = _i.next()).done); _n = true) { _arr.push(_s.value); if (i && _arr.length === i) break; } } catch (err) { _d = true; _e = err; } finally { try { if (!_n && _i["return"] != null) _i["return"](); } finally { if (_d) throw _e; } } return _arr; }

function _arrayWithHoles(arr) { if (Array.isArray(arr)) return arr; }

const COLLAPSERS = [require("./object-collapser"), require("./array-collapser"), require("./array-property-collapser"), require("./set-collapser")].map(Collapser => {
  return new Collapser();
});

function getFunctionParent(path, scopeParent) {
  const parent = path.findParent(p => p.isFunction()); // don"t traverse higher than the function the var is defined in.

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

function getIdAndFunctionReferences(name, parent) {
  // Returns false if there's an error. Otherwise returns a list of references.
  const binding = parent.scope.getBinding(name);

  if (!binding) {
    return false;
  }

  const references = binding.referencePaths.reduce((references, ref) => {
    references.add(ref);
    getFunctionReferences(ref, parent, references);
    return references;
  }, new Set());
  return Array.from(references);
}

function validateTopLevel(path) {
  // Ensures the structure is of the form (roughly):
  // {
  //   ...
  //   var foo = expr;
  //   ...
  // }
  // returns null if not of this form
  // otherwise returns [foo as string, ?rval, index of the variable declaration]
  const declarations = path.get("declarations");

  if (declarations.length !== 1) {
    return;
  }

  const declaration = declarations[0];
  const id = declaration.get("id"),
        init = declaration.get("init");

  if (!id.isIdentifier()) {
    return;
  }

  const parent = path.parentPath;

  if (!parent.isBlockParent() || !parent.isScopable()) {
    return;
  }

  const body = parent.get("body");

  if (!Array.isArray(body)) {
    return;
  }

  const startIndex = body.indexOf(path);

  if (startIndex === -1) {
    return;
  }

  return [id.node.name, init, startIndex];
}

function collectExpressions(path, isExprTypeValid) {
  // input: ExprStatement => 'a | SequenceExpression
  // SequenceExpression => 'a list
  // Validates 'a is of the right type
  // returns null if found inconsistency, else returns Array<"a>
  if (path.isExpressionStatement()) {
    const exprs = collectExpressions(path.get("expression"), isExprTypeValid);
    return exprs !== null ? exprs : null;
  }

  if (path.isSequenceExpression()) {
    const exprs = path.get("expressions").map(p => collectExpressions(p, isExprTypeValid));

    if (exprs.some(e => e === null)) {
      return null;
    } else {
      return exprs.reduce((s, n) => s.concat(n), []); // === Array.flatten
    }
  }

  if (isExprTypeValid(path)) {
    return [path];
  }

  return null;
}

function getContiguousStatementsAndExpressions(body, start, end, isExprTypeValid, checkExpr) {
  const statements = [];
  let allExprs = [];

  for (let i = start; i < end; i++) {
    const exprs = collectExpressions(body[i], isExprTypeValid);

    if (exprs === null || !exprs.every(e => checkExpr(e))) {
      break;
    }

    statements.push(body[i]);
    allExprs = allExprs.concat(exprs);
  }

  return [statements, allExprs];
}

function getReferenceChecker(references) {
  // returns a function s.t. given an expr, returns true iff expr is an ancestor of a reference
  return expr => references.some(r => r === expr || r.isDescendant(expr));
}

function tryUseCollapser(t, collapser, varDecl, topLevel, checkReference) {
  // Returns true iff successfully used the collapser. Otherwise returns undefined.
  const _topLevel = _slicedToArray(topLevel, 3),
        name = _topLevel[0],
        init = _topLevel[1],
        startIndex = _topLevel[2];

  if (!collapser.isInitTypeValid(init)) {
    return;
  }

  const body = varDecl.parentPath.get("body");

  const _getContiguousStateme = getContiguousStatementsAndExpressions(body, startIndex + 1, body.length, collapser.isExpressionTypeValid, collapser.getExpressionChecker(name, checkReference)),
        _getContiguousStateme2 = _slicedToArray(_getContiguousStateme, 2),
        statements = _getContiguousStateme2[0],
        exprs = _getContiguousStateme2[1];

  if (statements.length === 0) {
    return;
  }

  const assignments = exprs.map(e => collapser.extractAssignment(e));
  const oldInit = init.node;
  const newInit = t.cloneDeep(oldInit);

  if (!assignments.every(assignment => collapser.addSuccessfully(t, assignment, newInit))) {
    return;
  } // some collapses may increase the size


  if (!collapser.isSizeSmaller({
    newInit,
    oldInit,
    varDecl,
    assignments,
    statements
  })) {
    return;
  }

  init.replaceWith(newInit);
  statements.forEach(s => s.remove());
  return true;
}

module.exports = function ({
  types: t
}) {
  return {
    name: "transform-inline-consecutive-adds",
    visitor: {
      VariableDeclaration(varDecl) {
        const topLevel = validateTopLevel(varDecl);

        if (!topLevel) {
          return;
        }

        const _topLevel2 = _slicedToArray(topLevel, 1),
              name = _topLevel2[0];

        const references = getIdAndFunctionReferences(name, varDecl.parentPath);

        if (references === false) {
          return;
        }

        const checkReference = getReferenceChecker(references);

        if (COLLAPSERS.some(c => tryUseCollapser(t, c, varDecl, topLevel, checkReference))) {
          return;
        }
      }

    }
  };
};
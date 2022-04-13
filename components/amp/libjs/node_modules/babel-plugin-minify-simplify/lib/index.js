"use strict";

function _slicedToArray(arr, i) { return _arrayWithHoles(arr) || _iterableToArrayLimit(arr, i) || _nonIterableRest(); }

function _nonIterableRest() { throw new TypeError("Invalid attempt to destructure non-iterable instance"); }

function _iterableToArrayLimit(arr, i) { var _arr = []; var _n = true; var _d = false; var _e = undefined; try { for (var _i = arr[Symbol.iterator](), _s; !(_n = (_s = _i.next()).done); _n = true) { _arr.push(_s.value); if (i && _arr.length === i) break; } } catch (err) { _d = true; _e = err; } finally { try { if (!_n && _i["return"] != null) _i["return"](); } finally { if (_d) throw _e; } } return _arr; }

function _arrayWithHoles(arr) { if (Array.isArray(arr)) return arr; }

module.exports = ({
  types: t
}) => {
  const flipExpressions = require("babel-helper-flip-expressions")(t);

  const toMultipleSequenceExpressions = require("babel-helper-to-multiple-sequence-expressions")(t);

  const ifStatement = require("./if-statement")(t);

  const conditionalExpression = require("./conditional-expression")(t);

  const logicalExpression = require("./logical-expression")(t);

  const assignmentExpression = require("./assignment-expression")(t);

  const VOID_0 = t.unaryExpression("void", t.numericLiteral(0), true);
  const condExprSeen = Symbol("condExprSeen");
  const seqExprSeen = Symbol("seqExprSeen");
  const shouldRevisit = Symbol("shouldRevisit");
  return {
    name: "minify-simplify",
    visitor: {
      Statement: {
        exit(path) {
          if (path.node[shouldRevisit]) {
            delete path.node[shouldRevisit];
            path.visit();
          }
        }

      },
      // CallExpression(path) {
      // const { node } = path;

      /* (function() {})() -> !function() {}()
        There is a bug in babel in printing this. Disabling for now.
        if (t.isFunctionExpression(node.callee) &&
            (t.isExpressionStatement(parent) ||
             (t.isSequenceExpression(parent) && parent.expressions[0] === node))
        ) {
          path.replaceWith(
            t.callExpression(
              t.unaryExpression("!", node.callee, true),
              node.arguments
            )
          );
          return;
        }*/
      // },
      UnaryExpression: {
        enter: [// Demorgans.
        function (path) {
          const node = path.node;

          if (node.operator !== "!" || flipExpressions.hasSeen(node)) {
            return;
          }

          const expr = node.argument; // We need to make sure that the return type will always be boolean.

          if (!(t.isLogicalExpression(expr) || t.isConditionalExpression(expr) || t.isBinaryExpression(expr))) {
            return;
          }

          if (t.isBinaryExpression(expr) && t.COMPARISON_BINARY_OPERATORS.indexOf(expr.operator) === -1) {
            return;
          }

          if (flipExpressions.shouldFlip(expr, 1)) {
            const newNode = flipExpressions.flip(expr);
            path.replaceWith(newNode);
          }
        }, // !(a, b, c) -> a, b, !c
        function (path) {
          const node = path.node;

          if (node.operator !== "!") {
            return;
          }

          if (!t.isSequenceExpression(node.argument)) {
            return;
          }

          const seq = node.argument.expressions;
          const expr = seq[seq.length - 1];
          seq[seq.length - 1] = t.unaryExpression("!", expr, true);
          path.replaceWith(node.argument);
        }, // !(a ? b : c) -> a ? !b : !c
        function (path) {
          const node = path.node;

          if (node.operator !== "!") {
            return;
          }

          if (!t.isConditional(node.argument)) {
            return;
          }

          const cond = node.argument;
          cond.alternate = t.unaryExpression("!", cond.alternate, true);
          cond.consequent = t.unaryExpression("!", cond.consequent, true);
          path.replaceWith(node.argument);
        }]
      },

      BinaryExpression(path) {
        if (["!=", "=="].indexOf(path.node.operator) !== -1) {
          undefinedToNull(path.get("left"));
          undefinedToNull(path.get("right"));
        }
      },

      LogicalExpression: {
        exit: logicalExpression.simplifyPatterns
      },
      AssignmentExpression: assignmentExpression.simplify,
      ConditionalExpression: {
        enter: [// !foo ? 'foo' : 'bar' -> foo ? 'bar' : 'foo'
        // foo !== 'lol' ? 'foo' : 'bar' -> foo === 'lol' ? 'bar' : 'foo'
        function flipIfOrConditional(path) {
          const node = path.node;

          if (!path.get("test").isLogicalExpression()) {
            flipNegation(node);
            return;
          }

          if (flipExpressions.shouldFlip(node.test)) {
            node.test = flipExpressions.flip(node.test);
            var _ref = [node.consequent, node.alternate];
            node.alternate = _ref[0];
            node.consequent = _ref[1];
          }
        }, conditionalExpression.simplifyPatterns],
        exit: [// a ? x = foo : b ? x = bar : x = baz;
        // x = a ? foo : b ? bar : baz;
        function (topPath) {
          if (!topPath.parentPath.isExpressionStatement() && !topPath.parentPath.isSequenceExpression()) {
            return;
          }

          const mutations = [];
          let firstLeft = null;
          let operator = null;

          function visit(path) {
            if (path.isConditionalExpression()) {
              let bail = visit(path.get("consequent"));

              if (bail) {
                return true;
              }

              bail = visit(path.get("alternate"));
              return bail;
            }

            if (operator == null) {
              operator = path.node.operator;
            } else if (path.node.operator !== operator) {
              return true;
            }

            if (!path.isAssignmentExpression() || !(path.get("left").isIdentifier() || path.get("left").isMemberExpression())) {
              return true;
            }

            const left = path.get("left").node;

            if (firstLeft == null) {
              firstLeft = left;
            } else if (!t.isNodesEquivalent(left, firstLeft)) {
              return true;
            }

            mutations.push(() => path.replaceWith(path.get("right").node));
          }

          const bail = visit(topPath);

          if (bail) {
            return;
          }

          mutations.forEach(f => f());
          topPath.replaceWith(t.assignmentExpression(operator, firstLeft, topPath.node));
        }, // bar ? void 0 : void 0
        // (bar, void 0)
        // TODO: turn this into general equiv check
        function (path) {
          const node = path.node;

          if (isVoid0(node.consequent) && isVoid0(node.alternate)) {
            path.replaceWith(t.sequenceExpression([path.node.test, VOID_0]));
          }
        }, // bar ? void 0 : foo ? void 0 : <etc>
        // bar || foo : void 0
        // TODO: turn this into general equiv check
        function (path) {
          const node = path.node;

          if (node[condExprSeen] || !isVoid0(node.consequent)) {
            return;
          }

          node[condExprSeen] = true;
          const tests = [node.test];
          const mutations = [];
          let alt;

          for (let next = path.get("alternate"); next.isConditionalExpression(); next = next.get("alternate")) {
            next.node[condExprSeen] = true;
            alt = next.node.alternate;

            if (isVoid0(next.node.consequent)) {
              tests.push(next.node.test);
              mutations.push(() => next.remove());
            } else {
              alt = next.node;
              break;
            }
          }

          if (tests.length === 1) {
            return;
          }

          const test = tests.reduce((expr, curTest) => t.logicalExpression("||", expr, curTest));
          path.replaceWith(t.conditionalExpression(test, VOID_0, alt));
        }]
      },
      // concat
      VariableDeclaration: {
        enter: [// Put vars with no init at the top.
        function (path) {
          const node = path.node;

          if (node.declarations.length < 2) {
            return;
          }

          const inits = [];
          const empty = [];
          var _iteratorNormalCompletion = true;
          var _didIteratorError = false;
          var _iteratorError = undefined;

          try {
            for (var _iterator = node.declarations[Symbol.iterator](), _step; !(_iteratorNormalCompletion = (_step = _iterator.next()).done); _iteratorNormalCompletion = true) {
              const decl = _step.value;

              if (!decl.init) {
                empty.push(decl);
              } else {
                inits.push(decl);
              }
            } // This is based on exprimintation but there is a significant
            // imrpovement when we place empty vars at the top in smaller
            // files. Whereas it hurts in larger files.

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

          if (this.fitsInSlidingWindow) {
            node.declarations = empty.concat(inits);
          } else {
            node.declarations = inits.concat(empty);
          }
        }]
      },
      Function: {
        exit(path) {
          earlyReturnTransform(path);

          if (!path.node[shouldRevisit]) {
            return;
          }

          delete path.node[shouldRevisit];
          path.visit();
        }

      },
      For: {
        enter: earlyContinueTransform,
        exit: earlyContinueTransform
      },
      ForStatement: {
        // Merge previous expressions in the init part of the for.
        enter(path) {
          const node = path.node;

          if (!path.inList || node.init && !t.isExpression(node.init)) {
            return;
          }

          const prev = path.getSibling(path.key - 1);
          let consumed = false;

          if (prev.isVariableDeclaration()) {
            let referencedOutsideLoop = false; // we don't care if vars are referenced outside the loop as they are fn scope

            if (prev.node.kind === "let" || prev.node.kind === "const") {
              const ids = Object.keys(prev.getBindingIdentifiers());

              idloop: for (let i = 0; i < ids.length; i++) {
                const binding = prev.scope.bindings[ids[i]]; // TODO
                // Temporary Fix
                // if there is no binding, we assume it is referenced outside
                // and deopt to avoid bugs

                if (!binding) {
                  referencedOutsideLoop = true;
                  break idloop;
                }

                const refs = binding.referencePaths;

                for (let j = 0; j < refs.length; j++) {
                  if (!isAncestor(path, refs[j])) {
                    referencedOutsideLoop = true;
                    break idloop;
                  }
                }
              }
            }

            if (!node.init && !referencedOutsideLoop) {
              node.init = prev.node;
              consumed = true;
            }
          } else if (prev.isExpressionStatement()) {
            const expr = prev.node.expression;

            if (node.init) {
              if (t.isSequenceExpression(expr)) {
                expr.expressions.push(node.init);
                node.init = expr;
              } else {
                node.init = t.sequenceExpression([expr, node.init]);
              }
            } else {
              node.init = expr;
            }

            consumed = true;
          }

          if (consumed) {
            prev.remove();
          }
        },

        exit(path) {
          const node = path.node;

          if (!node.test) {
            return;
          }

          if (!path.get("body").isBlockStatement()) {
            const bodyNode = path.get("body").node;

            if (!t.isIfStatement(bodyNode)) {
              return;
            }

            if (t.isBreakStatement(bodyNode.consequent, {
              label: null
            })) {
              node.test = t.logicalExpression("&&", node.test, t.unaryExpression("!", bodyNode.test, true));
              node.body = bodyNode.alternate || t.emptyStatement();
              return;
            }

            if (t.isBreakStatement(bodyNode.alternate, {
              label: null
            })) {
              node.test = t.logicalExpression("&&", node.test, bodyNode.test);
              node.body = bodyNode.consequent || t.emptyStatement();
              return;
            }

            return;
          }

          const statements = node.body.body;
          const exprs = [];
          let ifStatement = null;
          let breakAt = null;
          let i = 0;

          for (let statement; statement = statements[i]; i++) {
            if (t.isIfStatement(statement)) {
              if (t.isBreakStatement(statement.consequent, {
                label: null
              })) {
                ifStatement = statement;
                breakAt = "consequent";
              } else if (t.isBreakStatement(statement.alternate, {
                label: null
              })) {
                ifStatement = statement;
                breakAt = "alternate";
              }

              break;
            } // A statement appears before the break statement then bail.


            if (!t.isExpressionStatement(statement)) {
              return;
            }

            exprs.push(statement.expression);
          }

          if (!ifStatement) {
            return;
          }

          const rest = [];

          if (breakAt === "consequent") {
            if (t.isBlockStatement(ifStatement.alternate)) {
              rest.push(...ifStatement.alternate.body);
            } else if (ifStatement.alternate) {
              rest.push(ifStatement.alternate);
            }
          } else {
            if (t.isBlockStatement(ifStatement.consequent)) {
              rest.push(...ifStatement.consequent.body);
            } else if (ifStatement.consequent) {
              rest.push(ifStatement.consequent);
            }
          }

          rest.push(...statements.slice(i + 1));
          const test = breakAt === "consequent" ? t.unaryExpression("!", ifStatement.test, true) : ifStatement.test;
          let expr;

          if (exprs.length === 1) {
            expr = t.sequenceExpression([exprs[0], test]);
          } else if (exprs.length) {
            exprs.push(test);
            expr = t.sequenceExpression(exprs);
          } else {
            expr = test;
          }

          node.test = t.logicalExpression("&&", node.test, expr);

          if (rest.length === 1) {
            node.body = rest[0];
          } else if (rest.length) {
            node.body = t.blockStatement(rest);
          } else {
            node.body = t.emptyStatement();
          }
        }

      },

      Program(path) {
        // An approximation of the resultant gzipped code after minification
        this.fitsInSlidingWindow = path.getSource().length / 10 < 33000;
        const node = path.node;
        const statements = toMultipleSequenceExpressions(node.body);

        if (!statements.length) {
          return;
        }

        node.body = statements;
      },

      BlockStatement: {
        enter(path) {
          const node = path.node,
                parent = path.parent;
          const top = [];
          const bottom = [];

          for (let i = 0; i < node.body.length; i++) {
            const bodyNode = node.body[i];

            if (t.isFunctionDeclaration(bodyNode)) {
              top.push(bodyNode);
            } else {
              bottom.push(bodyNode);
            }
          }

          const statements = top.concat(toMultipleSequenceExpressions(bottom));

          if (!statements.length) {
            return;
          }

          if (statements.length > 1 || needsBlock(node, parent) || node.directives) {
            node.body = statements;
            return;
          }

          if (statements.length) {
            path.replaceWith(statements[0]);
            return;
          }
        },

        exit(path) {
          const node = path.node,
                parent = path.parent;

          if (t.isArrowFunctionExpression(parent) && node.body.length === 1 && t.isReturnStatement(node.body[0]) && node.body[0].argument !== null) {
            path.replaceWith(node.body[0].argument);
            return;
          }

          if (needsBlock(node, parent)) {
            return;
          }

          if (node.body.length === 1) {
            path.get("body")[0].inList = false;
            path.replaceWith(node.body[0]);
            return;
          }

          if (node.body.length === 0) {
            path.replaceWith(t.emptyStatement());
            return;
          } // Check if oppurtinties to merge statements are available.


          const statements = node.body;

          if (!statements.length) {
            return;
          }

          var _iteratorNormalCompletion2 = true;
          var _didIteratorError2 = false;
          var _iteratorError2 = undefined;

          try {
            for (var _iterator2 = statements[Symbol.iterator](), _step2; !(_iteratorNormalCompletion2 = (_step2 = _iterator2.next()).done); _iteratorNormalCompletion2 = true) {
              const statement = _step2.value;

              if (!t.isExpressionStatement(statement)) {
                return;
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

          path.visit();
        }

      },
      ThrowStatement: createPrevExpressionEater("throw"),
      // Try to merge previous statements into a sequence
      ReturnStatement: {
        enter: [createPrevExpressionEater("return"), // Remove return if last statement with no argument.
        // Replace return with `void` argument with argument.
        function (path) {
          const node = path.node;

          if (!path.parentPath.parentPath.isFunction() || path.getSibling(path.key + 1).node) {
            return;
          }

          if (!node.argument) {
            path.remove();
            return;
          }

          if (t.isUnaryExpression(node.argument, {
            operator: "void"
          })) {
            path.replaceWith(node.argument.argument);
          }
        }]
      },
      // turn blocked ifs into single statements
      IfStatement: {
        exit: [ifStatement.mergeNestedIfs, ifStatement.simplify, ifStatement.switchConsequent, ifStatement.conditionalReturnToGuards, createPrevExpressionEater("if")]
      },

      WhileStatement(path) {
        const node = path.node;
        path.replaceWith(t.forStatement(null, node.test, null, node.body));
      },

      ForInStatement: createPrevExpressionEater("for-in"),
      // Flatten sequence expressions.
      SequenceExpression: {
        exit(path) {
          if (path.node[seqExprSeen]) {
            return;
          }

          function flatten(node) {
            node[seqExprSeen] = true;
            const ret = [];
            var _iteratorNormalCompletion3 = true;
            var _didIteratorError3 = false;
            var _iteratorError3 = undefined;

            try {
              for (var _iterator3 = node.expressions[Symbol.iterator](), _step3; !(_iteratorNormalCompletion3 = (_step3 = _iterator3.next()).done); _iteratorNormalCompletion3 = true) {
                const n = _step3.value;

                if (t.isSequenceExpression(n)) {
                  ret.push(...flatten(n));
                } else {
                  ret.push(n);
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

            return ret;
          }

          path.node.expressions = flatten(path.node);
        }

      },

      SwitchCase(path) {
        const node = path.node;

        if (!node.consequent.length) {
          return;
        }

        node.consequent = toMultipleSequenceExpressions(node.consequent);
      },

      SwitchStatement: {
        exit: [// Convert switch statements with all returns in their cases
        // to return conditional.
        function (path) {
          const node = path.node; // Need to be careful of side-effects.

          if (!t.isIdentifier(node.discriminant)) {
            return;
          }

          if (!node.cases.length) {
            return;
          }

          const consTestPairs = [];
          let fallThru = [];
          let defaultRet;
          var _iteratorNormalCompletion4 = true;
          var _didIteratorError4 = false;
          var _iteratorError4 = undefined;

          try {
            for (var _iterator4 = node.cases[Symbol.iterator](), _step4; !(_iteratorNormalCompletion4 = (_step4 = _iterator4.next()).done); _iteratorNormalCompletion4 = true) {
              const switchCase = _step4.value;

              if (switchCase.consequent.length > 1) {
                return;
              }

              const cons = switchCase.consequent[0]; // default case

              if (!switchCase.test) {
                if (!t.isReturnStatement(cons)) {
                  return;
                }

                defaultRet = cons;
                continue;
              }

              if (!switchCase.consequent.length) {
                fallThru.push(switchCase.test);
                continue;
              } // TODO: can we void it?


              if (!t.isReturnStatement(cons)) {
                return;
              }

              let test = t.binaryExpression("===", node.discriminant, switchCase.test);

              if (fallThru.length && !defaultRet) {
                test = fallThru.reduceRight((right, test) => t.logicalExpression("||", t.binaryExpression("===", node.discriminant, test), right), test);
              }

              fallThru = [];
              consTestPairs.push([test, cons.argument || VOID_0]);
            } // Bail if we have any remaining fallthrough

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

          if (fallThru.length) {
            return;
          } // We need the default to be there to make sure there is an oppurtinity
          // not to return.


          if (!defaultRet) {
            if (path.inList) {
              const nextPath = path.getSibling(path.key + 1);

              if (nextPath.isReturnStatement()) {
                defaultRet = nextPath.node;
                nextPath.remove();
              } else if (!nextPath.node && path.parentPath.parentPath.isFunction()) {
                // If this is the last statement in a function we just fake a void return.
                defaultRet = t.returnStatement(VOID_0);
              } else {
                return;
              }
            } else {
              return;
            }
          }

          const cond = consTestPairs.reduceRight((alt, [test, cons]) => t.conditionalExpression(test, cons, alt), defaultRet.argument || VOID_0);
          path.replaceWith(t.returnStatement(cond)); // Maybe now we can merge with some previous switch statement.

          if (path.inList) {
            const prev = path.getSibling(path.key - 1);

            if (prev.isSwitchStatement()) {
              prev.visit();
            }
          }
        }, // Convert switches into conditionals.
        function (path) {
          const node = path.node; // Need to be careful of side-effects.

          if (!t.isIdentifier(node.discriminant)) {
            return;
          }

          if (!node.cases.length) {
            return;
          }

          const exprTestPairs = [];
          let fallThru = [];
          let defaultExpr;
          var _iteratorNormalCompletion5 = true;
          var _didIteratorError5 = false;
          var _iteratorError5 = undefined;

          try {
            for (var _iterator5 = node.cases[Symbol.iterator](), _step5; !(_iteratorNormalCompletion5 = (_step5 = _iterator5.next()).done); _iteratorNormalCompletion5 = true) {
              const switchCase = _step5.value;

              if (!switchCase.test) {
                if (switchCase.consequent.length !== 1) {
                  return;
                }

                if (!t.isExpressionStatement(switchCase.consequent[0])) {
                  return;
                }

                defaultExpr = switchCase.consequent[0].expression;
                continue;
              }

              if (!switchCase.consequent.length) {
                fallThru.push(switchCase.test);
                continue;
              }

              const _switchCase$consequen = _slicedToArray(switchCase.consequent, 2),
                    cons = _switchCase$consequen[0],
                    breakStatement = _switchCase$consequen[1];

              if (switchCase === node.cases[node.cases.length - 1]) {
                if (breakStatement && !t.isBreakStatement(breakStatement)) {
                  return;
                }
              } else if (!t.isBreakStatement(breakStatement)) {
                return;
              }

              if (!t.isExpressionStatement(cons) || switchCase.consequent.length > 2) {
                return;
              }

              let test = t.binaryExpression("===", node.discriminant, switchCase.test);

              if (fallThru.length && !defaultExpr) {
                test = fallThru.reduceRight((right, test) => t.logicalExpression("||", t.binaryExpression("===", node.discriminant, test), right), test);
              }

              fallThru = [];
              exprTestPairs.push([test, cons.expression]);
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

          if (fallThru.length) {
            return;
          }

          const cond = exprTestPairs.reduceRight((alt, [test, cons]) => t.conditionalExpression(test, cons, alt), defaultExpr || VOID_0);
          path.replaceWith(cond);
        }, function (path) {
          const node = path.node;

          if (!node.cases.length) {
            return;
          }

          const lastCase = path.get("cases")[node.cases.length - 1];

          if (!lastCase.node.consequent.length) {
            return;
          }

          const potentialBreak = lastCase.get("consequent")[lastCase.node.consequent.length - 1];

          if (t.isBreakStatement(potentialBreak) && potentialBreak.node.label === null) {
            potentialBreak.remove();
          }
        }, createPrevExpressionEater("switch")]
      }
    }
  };

  function flipNegation(node) {
    if (!node.consequent || !node.alternate) {
      return;
    }

    const test = node.test;
    let flip = false;

    if (t.isBinaryExpression(test)) {
      if (test.operator === "!==") {
        test.operator = "===";
        flip = true;
      }

      if (test.operator === "!=") {
        test.operator = "==";
        flip = true;
      }
    }

    if (t.isUnaryExpression(test, {
      operator: "!"
    })) {
      node.test = test.argument;
      flip = true;
    }

    if (flip) {
      const consequent = node.consequent;
      node.consequent = node.alternate;
      node.alternate = consequent;
    }
  }

  function needsBlock(node, parent) {
    return t.isFunction(parent) && node === parent.body || t.isTryStatement(parent) || t.isCatchClause(parent) || t.isSwitchStatement(parent) || isSingleBlockScopeDeclaration(node) && (t.isIfStatement(parent) || t.isLoop(parent));
  }

  function isSingleBlockScopeDeclaration(block) {
    return t.isBlockStatement(block) && block.body.length === 1 && (t.isVariableDeclaration(block.body[0], {
      kind: "let"
    }) || t.isVariableDeclaration(block.body[0], {
      kind: "const"
    }) || t.isFunctionDeclaration(block.body[0]));
  }

  function isVoid0(expr) {
    return expr === VOID_0 || t.isUnaryExpression(expr, {
      operator: "void"
    }) && t.isNumericLiteral(expr.argument, {
      value: 0
    });
  }

  function earlyReturnTransform(path) {
    const block = path.get("body");

    if (!block.isBlockStatement()) {
      return;
    }

    const body = block.get("body");

    for (let i = body.length - 1; i >= 0; i--) {
      const statement = body[i];

      if (t.isIfStatement(statement.node) && !statement.node.alternate && t.isReturnStatement(statement.node.consequent) && !statement.node.consequent.argument) {
        genericEarlyExitTransform(statement);
      }
    }
  }

  function earlyContinueTransform(path) {
    const block = path.get("body");

    if (!block.isBlockStatement()) {
      return;
    }

    let body = block.get("body");

    for (let i = body.length - 1; i >= 0; i--) {
      const statement = body[i];

      if (t.isIfStatement(statement.node) && !statement.node.alternate && t.isContinueStatement(statement.node.consequent) && !statement.node.consequent.label) {
        genericEarlyExitTransform(statement);
      }
    } // because we might have folded or removed statements


    body = block.get("body"); // We may have reduced the body to a single statement.

    if (body.length === 1 && !needsBlock(block.node, path.node)) {
      block.replaceWith(body[0].node);
    }
  }

  function genericEarlyExitTransform(path) {
    const node = path.node;
    const statements = path.parentPath.get(path.listKey).slice(path.key + 1).filter(stmt => !stmt.isFunctionDeclaration()); // deopt for any block scoped bindings
    // issue#399

    const deopt = !statements.every(stmt => {
      if (!(stmt.isVariableDeclaration({
        kind: "let"
      }) || stmt.isVariableDeclaration({
        kind: "const"
      }))) {
        return true;
      }

      const ids = Object.keys(stmt.getBindingIdentifiers());

      for (var _i2 = 0; _i2 < ids.length; _i2++) {
        const id = ids[_i2];
        const binding = path.scope.getBinding(id); // TODO
        // Temporary Fix
        // if there is no binding, we assume it is referenced outside
        // and deopt to avoid bugs

        if (!binding) {
          return false;
        }

        const refs = [...binding.referencePaths, ...binding.constantViolations];

        for (var _i3 = 0; _i3 < refs.length; _i3++) {
          const ref = refs[_i3];
          if (!ref.isIdentifier()) return false;
          const fnParent = ref.getFunctionParent(); // TODO
          // Usage of scopes and bindings in simplify plugin results in
          // undefined bindings because scope changes are not updated in the
          // scope tree. So, we deopt whenever we encounter one such issue
          // and not perform the transformation

          if (!fnParent) {
            return false;
          }

          if (fnParent.scope !== path.scope) return false;
        }
      }

      return true;
    });

    if (deopt) {
      path.visit();
      return false;
    }

    if (!statements.length) {
      path.replaceWith(t.expressionStatement(node.test));
      return;
    }

    const test = node.test;

    if (t.isBinaryExpression(test) && test.operator === "!==") {
      test.operator = "===";
    } else if (t.isBinaryExpression(test) && test.operator === "!=") {
      test.operator = "==";
    } else if (t.isUnaryExpression(test, {
      operator: "!"
    })) {
      node.test = test.argument;
    } else {
      node.test = t.unaryExpression("!", node.test, true);
    }

    path.get("consequent").replaceWith(t.blockStatement(statements.map(stmt => t.clone(stmt.node))));
    let l = statements.length;

    while (l-- > 0) {
      if (!statements[l].isFunctionDeclaration()) {
        path.getSibling(path.key + 1).remove();
      }
    } // this should take care of removing the block


    path.visit();
  }

  function createPrevExpressionEater(keyword) {
    let key;

    switch (keyword) {
      case "switch":
        key = "discriminant";
        break;

      case "throw":
      case "return":
        key = "argument";
        break;

      case "if":
        key = "test";
        break;

      case "for-in":
        key = "right";
        break;
    }

    return function (path) {
      if (!path.inList) {
        return;
      }

      const node = path.node;
      const prev = path.getSibling(path.key - 1);

      if (!prev.isExpressionStatement()) {
        return;
      }

      let seq = prev.node.expression;

      if (node[key]) {
        if (t.isSequenceExpression(seq)) {
          seq.expressions.push(node[key]);
        } else {
          seq = t.sequenceExpression([seq, node[key]]);
        }
      } else {
        if (t.isSequenceExpression(seq)) {
          const lastExpr = seq.expressions[seq.expressions.length - 1];
          seq.expressions[seq.expressions.length - 1] = t.unaryExpression("void", lastExpr, true);
        } else {
          seq = t.unaryExpression("void", seq, true);
        }
      }

      if (seq) {
        node[key] = seq;
        prev.remove(); // Since we were able to merge some stuff it's possible that this has opened
        // oppurtinties for other transforms to happen.
        // TODO: Look into changing the traversal order from bottom to up to avoid
        // having to revisit things.

        if (path.parentPath.parent) {
          path.parentPath.parent[shouldRevisit] = true;
        }
      }
    };
  } // path1 -> path2
  // is path1 an ancestor of path2


  function isAncestor(path1, path2) {
    return !!path2.findParent(parent => parent === path1);
  }

  function isPureVoid(path) {
    return path.isUnaryExpression({
      operator: "void"
    }) && path.isPure();
  }

  function isGlobalUndefined(path) {
    return path.isIdentifier({
      name: "undefined"
    }) && !path.scope.getBinding("undefined");
  }

  function undefinedToNull(path) {
    if (isGlobalUndefined(path) || isPureVoid(path)) {
      path.replaceWith(t.nullLiteral());
    }
  }
};
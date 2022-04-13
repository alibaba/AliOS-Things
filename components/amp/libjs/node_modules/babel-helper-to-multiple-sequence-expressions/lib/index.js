"use strict";

module.exports = function (t) {
  return function toMultipleSequenceExpressions(statements) {
    const retStatements = [];
    let bailed;

    do {
      const res = convert(statements);
      bailed = res.bailed;
      const seq = res.seq,
            bailedAtIndex = res.bailedAtIndex;

      if (seq) {
        retStatements.push(t.expressionStatement(seq));
      }

      if (bailed && statements[bailedAtIndex]) {
        retStatements.push(statements[bailedAtIndex]);
      }

      if (bailed) {
        statements = statements.slice(bailedAtIndex + 1);

        if (!statements.length) {
          bailed = false;
        }
      }
    } while (bailed);

    return retStatements;

    function convert(nodes) {
      const exprs = [];
      const comments = [];

      for (let i = 0; i < nodes.length; i++) {
        const bail = () => {
          let seq;

          if (exprs.length === 1) {
            seq = exprs[0];
          } else if (exprs.length) {
            seq = t.sequenceExpression(exprs);
          }

          return {
            seq,
            bailed: true,
            bailedAtIndex: i
          };
        };

        const node = nodes[i];

        if (t.isExpression(node)) {
          exprs.push(node);
        } else if (t.isExpressionStatement(node)) {
          if (node.leadingComments) {
            comments.push(...node.leadingComments);
          }

          if (node.expression) exprs.push(node.expression);
        } else if (t.isIfStatement(node)) {
          let consequent;

          if (node.consequent) {
            const res = convert([node.consequent]);

            if (res.bailed) {
              return bail();
            }

            consequent = res.seq;
          }

          let alternate;

          if (node.alternate) {
            const res = convert([node.alternate]);

            if (res.bailed) {
              return bail();
            }

            alternate = res.seq;
          }

          if (!alternate && !consequent) {
            exprs.push(node.test);
          } else if (!alternate) {
            exprs.push(t.logicalExpression("&&", node.test, consequent));
          } else if (!consequent) {
            exprs.push(t.logicalExpression("||", node.test, alternate));
          } else {
            exprs.push(t.conditionalExpression(node.test, consequent, alternate));
          }
        } else if (t.isBlockStatement(node)) {
          const res = convert(node.body);

          if (res.bailed) {
            return bail();
          }

          if (res.seq) {
            exprs.push(res.seq);
          }
        } else {
          return bail();
        }
      }

      let seq;

      if (exprs.length === 1) {
        seq = exprs[0];
      } else if (exprs.length) {
        seq = t.sequenceExpression(exprs);
      }
      /**
       * collect all the comment ast nodes that are before expression
       * statments and add it to the new generated node
       */


      if (seq) {
        seq.leadingComments = comments;
      }
      /* eslint-disable no-self-assign */


      seq = seq;
      return {
        seq
      };
    }
  };
};
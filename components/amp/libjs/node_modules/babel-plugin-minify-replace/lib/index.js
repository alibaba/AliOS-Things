"use strict";

module.exports = ({
  types: t
}) => {
  const NO_MEMBER = Symbol("no member");
  const replaceVisitor = {
    ReferencedIdentifier(path) {
      const _path = path,
            node = _path.node;
      const optionsMap = this.replacements[node.name];

      if (!optionsMap) {
        return;
      }

      let options;

      if (path.parentPath.isMemberExpression({
        object: node
      })) {
        const property = path.parent.property;
        const key = t.isIdentifier(property) && property.name;

        if (typeof key === "string") {
          options = optionsMap[key];
          path = path.parentPath;
        }
      }

      if (!options) {
        options = optionsMap[NO_MEMBER];
      }

      if (!options) {
        return;
      }

      path.replaceWith(options.node);
    }

  };
  return {
    name: "minify-replace",
    visitor: {
      Program(path) {
        /**
           Replacements is an array of objects like this:
           {
             identifierName: 'console',
             member: 'log', // optional
             replacement: {
               type: 'identifier',
               value: '',
             },
           }
        **/
        if (!this.opts.replacements) {
          // No replacements. Bail.
          return;
        }

        const map = Object.create(null);
        this.opts.replacements.forEach(({
          identifierName,
          replacement,
          member
        }) => {
          if (path.scope.globals[identifierName]) {
            // Convert to a node, we only allow identifiers and literals as replacements
            if (!replacement.type.match(/literal|identifier/i)) {
              throw new Error("Only literals and identifier are supported as replacements");
            }

            const node = t[replacement.type](replacement.value);
            const options = {
              identifierName,
              node,
              member
            };

            if (!map[identifierName]) {
              map[identifierName] = {};
            }

            if (member && map[identifierName][member]) {
              throw new Error(`Replacement collision ${identifierName}.${member}`);
            }

            map[identifierName][member || NO_MEMBER] = options;
          }
        });
        path.traverse(replaceVisitor, {
          replacements: map
        });
      }

    }
  };
};
#-----------------------------------------------------------------
# pycparser: func_defs_add_param.py
#
# Example of rewriting AST nodes to add parameters to function
# definitions. Adds an "int _hidden" to every function.
#
# Eli Bendersky [https://eli.thegreenplace.net/]
# License: BSD
#-----------------------------------------------------------------
from __future__ import print_function
import sys

from pycparser import c_parser, c_ast, c_generator

text = r"""
void foo(int a, int b) {
}

void bar() {
}
"""


class ParamAdder(c_ast.NodeVisitor):
    def visit_FuncDecl(self, node):
        ty = c_ast.TypeDecl(declname='_hidden',
                            quals=[],
                            type=c_ast.IdentifierType(['int']))
        newdecl = c_ast.Decl(
                    name='_hidden',
                    quals=[],
                    storage=[],
                    funcspec=[],
                    type=ty,
                    init=None,
                    bitsize=None,
                    coord=node.coord)
        if node.args:
            node.args.params.append(newdecl)
        else:
            node.args = c_ast.ParamList(params=[newdecl])
        

if __name__ == '__main__':
    parser = c_parser.CParser()
    ast = parser.parse(text)
    print("AST before change:")
    ast.show(offset=2)

    v = ParamAdder()
    v.visit(ast)

    print("\nAST after change:")
    ast.show(offset=2)

    print("\nCode after change:")
    generator = c_generator.CGenerator()
    print(generator.visit(ast))

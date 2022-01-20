from __future__ import print_function

import sys
from pycparser import c_parser, c_generator, c_ast, parse_file


if __name__ == "__main__":
    parser = c_parser.CParser()
    code = r'''
    void* ptr = (int[ ]){0};
    '''

    print(code)
    ast = parser.parse(code)
    ast.show(attrnames=True, nodenames=True)
    print(ast.ext[0].__slots__)
    print(dir(ast.ext[0]))

    print("==== From C generator:")
    generator = c_generator.CGenerator()
    print(generator.visit(ast))

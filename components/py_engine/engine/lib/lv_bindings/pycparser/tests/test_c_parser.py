#!/usr/bin/env python

import pprint
import re
import os, sys
import io
import unittest

sys.path[0:0] = ['.', '..']

from pycparser import c_parser
from pycparser.c_ast import *
from pycparser.c_parser import CParser, Coord, ParseError

_c_parser = c_parser.CParser(
                lex_optimize=False,
                yacc_debug=True,
                yacc_optimize=False,
                yacctab='yacctab')


def expand_decl(decl):
    """ Converts the declaration into a nested list.
    """
    typ = type(decl)

    if typ == TypeDecl:
        return ['TypeDecl', expand_decl(decl.type)]
    elif typ == IdentifierType:
        return ['IdentifierType', decl.names]
    elif typ == ID:
        return ['ID', decl.name]
    elif typ in [Struct, Union]:
        decls = [expand_decl(d) for d in decl.decls or []]
        return [typ.__name__, decl.name, decls]
    elif typ == Enum:
        if decl.values is None:
            values = None
        else:
            assert isinstance(decl.values, EnumeratorList)
            values = [enum.name for enum in decl.values.enumerators]
        return ['Enum', decl.name, values]
    else:
        nested = expand_decl(decl.type)

        if typ == Decl:
            if decl.quals:
                return ['Decl', decl.quals, decl.name, nested]
            else:
                return ['Decl', decl.name, nested]
        elif typ == Typename: # for function parameters
            if decl.quals:
                return ['Typename', decl.quals, nested]
            else:
                return ['Typename', nested]
        elif typ == ArrayDecl:
            dimval = decl.dim.value if decl.dim else ''
            return ['ArrayDecl', dimval, decl.dim_quals, nested]
        elif typ == PtrDecl:
            if decl.quals:
                return ['PtrDecl', decl.quals, nested]
            else:
                return ['PtrDecl', nested]
        elif typ == Typedef:
            return ['Typedef', decl.name, nested]
        elif typ == FuncDecl:
            if decl.args:
                params = [expand_decl(param) for param in decl.args.params]
            else:
                params = []
            return ['FuncDecl', params, nested]


def expand_init(init):
    """ Converts an initialization into a nested list
    """
    typ = type(init)

    if typ == NamedInitializer:
        des = [expand_init(dp) for dp in init.name]
        return (des, expand_init(init.expr))
    elif typ in (InitList, ExprList):
        return [expand_init(expr) for expr in init.exprs]
    elif typ == Constant:
        return ['Constant', init.type, init.value]
    elif typ == ID:
        return ['ID', init.name]
    elif typ == UnaryOp:
        return ['UnaryOp', init.op, expand_decl(init.expr)]


class TestCParser_base(unittest.TestCase):
    def parse(self, txt, filename=''):
        return self.cparser.parse(txt, filename)

    def setUp(self):
        self.cparser = _c_parser

    def assert_coord(self, node, line, column=None, file=None):
        self.assertEqual(node.coord.line, line)
        if column is not None:
            self.assertEqual(node.coord.column, column)
        if file:
            self.assertEqual(node.coord.file, file)



class TestCParser_fundamentals(TestCParser_base):
    def get_decl(self, txt, index=0):
        """ Given a source and an index returns the expanded
            declaration at that index.

            FileAST holds a list of 'external declarations'.
            index is the offset of the desired declaration in that
            list.
        """
        t = self.parse(txt).ext[index]
        return expand_decl(t)

    def get_decl_init(self, txt, index=0):
        """ Returns the expanded initializer of the declaration
            at index.
        """
        t = self.parse(txt).ext[index]
        return expand_init(t.init)

    def test_FileAST(self):
        t = self.parse('int a; char c;')
        self.assertIsInstance(t, FileAST)
        self.assertEqual(len(t.ext), 2)

        # empty file
        t2 = self.parse('')
        self.assertIsInstance(t2, FileAST)
        self.assertEqual(len(t2.ext), 0)

    def test_empty_toplevel_decl(self):
        code = 'int foo;;'
        t = self.parse(code)
        self.assertIsInstance(t, FileAST)
        self.assertEqual(len(t.ext), 1)
        self.assertEqual(self.get_decl(code),
            ['Decl', 'foo',
                ['TypeDecl', ['IdentifierType', ['int']]]])

    def test_initial_semi(self):
        t = self.parse(';')
        self.assertEqual(len(t.ext), 0)
        t = self.parse(';int foo;')
        self.assertEqual(len(t.ext), 1)
        self.assertEqual(expand_decl(t.ext[0]),
            ['Decl', 'foo',
                ['TypeDecl', ['IdentifierType', ['int']]]])

    def test_coords(self):
        """ Tests the "coordinates" of parsed elements - file
            name, line and column numbers, with modification
            insterted by #line directives.
        """
        self.assert_coord(self.parse('int a;').ext[0], 1, 5)

        t1 = """
        int a;
        int b;\n\n
        int c;
        """
        f1 = self.parse(t1, filename='test.c')
        self.assert_coord(f1.ext[0], 2, 13, 'test.c')
        self.assert_coord(f1.ext[1], 3, 13, 'test.c')
        self.assert_coord(f1.ext[2], 6, 13, 'test.c')

        t1_1 = '''
        int main() {
            k = p;
            printf("%d", b);
            return 0;
        }'''
        f1_1 = self.parse(t1_1, filename='test.c')
        self.assert_coord(f1_1.ext[0].body.block_items[0], 3, 13, 'test.c')
        self.assert_coord(f1_1.ext[0].body.block_items[1], 4, 13, 'test.c')

        t1_2 = '''
        int main () {
            int p = (int) k;
        }'''
        f1_2 = self.parse(t1_2, filename='test.c')
        # make sure that the Cast has a coord (issue 23)
        self.assert_coord(f1_2.ext[0].body.block_items[0].init, 3, 21, file='test.c')

        t2 = """
        #line 99
        int c;
        """
        self.assert_coord(self.parse(t2).ext[0], 99, 13)

        t3 = """
        int dsf;
        char p;
        #line 3000 "in.h"
        char d;
        """
        f3 = self.parse(t3, filename='test.c')
        self.assert_coord(f3.ext[0], 2, 13, 'test.c')
        self.assert_coord(f3.ext[1], 3, 14, 'test.c')
        self.assert_coord(f3.ext[2], 3000, 14, 'in.h')

        t4 = """
        #line 20 "restore.h"
        int maydler(char);

        #line 30 "includes/daween.ph"
        long j, k;

        #line 50000
        char* ro;
        """
        f4 = self.parse(t4, filename='myb.c')
        self.assert_coord(f4.ext[0], 20, 13, 'restore.h')
        self.assert_coord(f4.ext[1], 30, 14, 'includes/daween.ph')
        self.assert_coord(f4.ext[2], 30, 17, 'includes/daween.ph')
        self.assert_coord(f4.ext[3], 50000, 13, 'includes/daween.ph')

        t5 = """
        int
        #line 99
        c;
        """
        self.assert_coord(self.parse(t5).ext[0], 99, 9)

        # coord for ellipsis
        t6 = """
        int foo(int j,
                ...) {
        }"""
        f6 = self.parse(t6, filename='z.c')
        self.assert_coord(self.parse(t6).ext[0].decl.type.args.params[1], 3, 17)

    def test_forloop_coord(self):
        t = '''\
        void foo() {
            for(int z=0; z<4;
                z++){}
        }
        '''
        s = self.parse(t, filename='f.c')
        forloop = s.ext[0].body.block_items[0]
        self.assert_coord(forloop.init, 2, 13, 'f.c')
        self.assert_coord(forloop.cond, 2, 26, 'f.c')
        self.assert_coord(forloop.next, 3, 17, 'f.c')

    def test_simple_decls(self):
        self.assertEqual(self.get_decl('int a;'),
            ['Decl', 'a', ['TypeDecl', ['IdentifierType', ['int']]]])

        self.assertEqual(self.get_decl('unsigned int a;'),
            ['Decl', 'a', ['TypeDecl', ['IdentifierType', ['unsigned', 'int']]]])

        self.assertEqual(self.get_decl('_Bool a;'),
            ['Decl', 'a', ['TypeDecl', ['IdentifierType', ['_Bool']]]])

        self.assertEqual(self.get_decl('float _Complex fcc;'),
            ['Decl', 'fcc', ['TypeDecl', ['IdentifierType', ['float', '_Complex']]]])

        self.assertEqual(self.get_decl('char* string;'),
            ['Decl', 'string',
                ['PtrDecl', ['TypeDecl', ['IdentifierType', ['char']]]]])

        self.assertEqual(self.get_decl('long ar[15];'),
            ['Decl', 'ar',
                ['ArrayDecl', '15', [],
                    ['TypeDecl', ['IdentifierType', ['long']]]]])

        self.assertEqual(self.get_decl('long long ar[15];'),
            ['Decl', 'ar',
                ['ArrayDecl', '15', [],
                    ['TypeDecl', ['IdentifierType', ['long', 'long']]]]])

        self.assertEqual(self.get_decl('unsigned ar[];'),
            ['Decl', 'ar',
                ['ArrayDecl', '', [],
                    ['TypeDecl', ['IdentifierType', ['unsigned']]]]])

        self.assertEqual(self.get_decl('int strlen(char* s);'),
            ['Decl', 'strlen',
                ['FuncDecl',
                    [['Decl', 's',
                        ['PtrDecl',
                            ['TypeDecl', ['IdentifierType', ['char']]]]]],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])

        self.assertEqual(self.get_decl('int strcmp(char* s1, char* s2);'),
            ['Decl', 'strcmp',
                ['FuncDecl',
                    [   ['Decl', 's1',
                            ['PtrDecl', ['TypeDecl', ['IdentifierType', ['char']]]]],
                        ['Decl', 's2',
                            ['PtrDecl', ['TypeDecl', ['IdentifierType', ['char']]]]]
                    ],
                ['TypeDecl', ['IdentifierType', ['int']]]]])

        # function return values and parameters may not have type information
        self.assertEqual(self.get_decl('extern foobar(foo, bar);'),
            ['Decl', 'foobar',
                ['FuncDecl',
                    [   ['ID', 'foo'],
                        ['ID', 'bar']
                    ],
                ['TypeDecl', ['IdentifierType', ['int']]]]])

    def test_int128(self):
        self.assertEqual(self.get_decl('__int128 a;'),
            ['Decl', 'a', ['TypeDecl', ['IdentifierType', ['__int128']]]])


    def test_nested_decls(self): # the fun begins
        self.assertEqual(self.get_decl('char** ar2D;'),
            ['Decl', 'ar2D',
                ['PtrDecl', ['PtrDecl',
                    ['TypeDecl', ['IdentifierType', ['char']]]]]])

        self.assertEqual(self.get_decl('int (*a)[1][2];'),
            ['Decl', 'a',
                ['PtrDecl',
                    ['ArrayDecl', '1', [],
                        ['ArrayDecl', '2', [],
                        ['TypeDecl', ['IdentifierType', ['int']]]]]]])

        self.assertEqual(self.get_decl('int *a[1][2];'),
            ['Decl', 'a',
                ['ArrayDecl', '1', [],
                    ['ArrayDecl', '2', [],
                        ['PtrDecl', ['TypeDecl', ['IdentifierType', ['int']]]]]]])

        self.assertEqual(self.get_decl('char* const* p;'),
            ['Decl', 'p',
                ['PtrDecl', ['PtrDecl', ['const'],
                    ['TypeDecl', ['IdentifierType', ['char']]]]]])

        self.assertEqual(self.get_decl('char* * const p;'),
            ['Decl', 'p',
                ['PtrDecl', ['const'], ['PtrDecl',
                    ['TypeDecl', ['IdentifierType', ['char']]]]]])

        self.assertEqual(self.get_decl('char ***ar3D[40];'),
            ['Decl', 'ar3D',
                ['ArrayDecl', '40', [],
                    ['PtrDecl', ['PtrDecl', ['PtrDecl',
                        ['TypeDecl', ['IdentifierType', ['char']]]]]]]])

        self.assertEqual(self.get_decl('char (***ar3D)[40];'),
            ['Decl', 'ar3D',
                ['PtrDecl', ['PtrDecl', ['PtrDecl',
                    ['ArrayDecl', '40', [], ['TypeDecl', ['IdentifierType', ['char']]]]]]]])

        self.assertEqual(self.get_decl('int (*x[4])(char, int);'),
            ['Decl', 'x',
                ['ArrayDecl', '4', [],
                    ['PtrDecl',
                        ['FuncDecl',
                            [   ['Typename',  ['TypeDecl', ['IdentifierType', ['char']]]],
                                ['Typename', ['TypeDecl', ['IdentifierType', ['int']]]]],
                            ['TypeDecl', ['IdentifierType', ['int']]]]]]])

        self.assertEqual(self.get_decl('char *(*(**foo [][8])())[];'),
            ['Decl', 'foo',
                ['ArrayDecl', '', [],
                    ['ArrayDecl', '8', [],
                        ['PtrDecl', ['PtrDecl',
                            ['FuncDecl',
                                [],
                                ['PtrDecl',
                                    ['ArrayDecl', '', [],
                                        ['PtrDecl',
                                            ['TypeDecl',
                                                ['IdentifierType', ['char']]]]]]]]]]]])

        # explore named and unnamed function pointer parameters,
        # with and without qualifiers

        # unnamed w/o quals
        self.assertEqual(self.get_decl('int (*k)(int);'),
            ['Decl', 'k',
                ['PtrDecl',
                    ['FuncDecl',
                        [['Typename', ['TypeDecl', ['IdentifierType', ['int']]]]],
                        ['TypeDecl', ['IdentifierType', ['int']]]]]])

        # unnamed w/ quals
        self.assertEqual(self.get_decl('int (*k)(const int);'),
            ['Decl', 'k',
                ['PtrDecl',
                    ['FuncDecl',
                        [['Typename', ['const'], ['TypeDecl', ['IdentifierType', ['int']]]]],
                        ['TypeDecl', ['IdentifierType', ['int']]]]]])

        # named w/o quals
        self.assertEqual(self.get_decl('int (*k)(int q);'),
            ['Decl', 'k',
                ['PtrDecl',
                    ['FuncDecl',
                        [['Decl', 'q', ['TypeDecl', ['IdentifierType', ['int']]]]],
                        ['TypeDecl', ['IdentifierType', ['int']]]]]])

        # named w/ quals
        self.assertEqual(self.get_decl('int (*k)(const volatile int q);'),
            ['Decl', 'k',
                ['PtrDecl',
                    ['FuncDecl',
                        [['Decl', ['const', 'volatile'], 'q',
                            ['TypeDecl', ['IdentifierType', ['int']]]]],
                        ['TypeDecl', ['IdentifierType', ['int']]]]]])

        self.assertEqual(self.get_decl('int (*k)(const volatile int* q);'),
            ['Decl', 'k',
                ['PtrDecl',
                    ['FuncDecl',
                        [['Decl', ['const', 'volatile'], 'q',
                            ['PtrDecl', ['TypeDecl', ['IdentifierType', ['int']]]]]],
                        ['TypeDecl', ['IdentifierType', ['int']]]]]])

        # restrict qualifier
        self.assertEqual(self.get_decl('int (*k)(restrict int* q);'),
            ['Decl', 'k',
                ['PtrDecl',
                    ['FuncDecl',
                        [['Decl', ['restrict'], 'q',
                            ['PtrDecl',
                                ['TypeDecl', ['IdentifierType', ['int']]]]]],
                        ['TypeDecl', ['IdentifierType', ['int']]]]]])

    def test_func_decls_with_array_dim_qualifiers(self):
        # named function parameter
        self.assertEqual(self.get_decl('int zz(int p[static 10]);'),
            ['Decl', 'zz',
                ['FuncDecl',
                    [['Decl', 'p', ['ArrayDecl', '10', ['static'],
                                       ['TypeDecl', ['IdentifierType', ['int']]]]]],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])

        self.assertEqual(self.get_decl('int zz(int p[const 10]);'),
            ['Decl', 'zz',
                ['FuncDecl',
                    [['Decl', 'p', ['ArrayDecl', '10', ['const'],
                                       ['TypeDecl', ['IdentifierType', ['int']]]]]],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])

        self.assertEqual(self.get_decl('int zz(int p[restrict][5]);'),
            ['Decl', 'zz',
                ['FuncDecl',
                    [['Decl', 'p', ['ArrayDecl', '', ['restrict'],
                        ['ArrayDecl', '5', [],
                            ['TypeDecl', ['IdentifierType', ['int']]]]]]],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])

        self.assertEqual(self.get_decl('int zz(int p[const restrict static 10][5]);'),
            ['Decl', 'zz',
                ['FuncDecl',
                    [['Decl', 'p', ['ArrayDecl', '10', ['const', 'restrict', 'static'],
                        ['ArrayDecl', '5', [],
                            ['TypeDecl', ['IdentifierType', ['int']]]]]]],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])

        # unnamed function parameter
        self.assertEqual(self.get_decl('int zz(int [const 10]);'),
            ['Decl', 'zz',
                ['FuncDecl',
                    [['Typename', ['ArrayDecl', '10', ['const'],
                                       ['TypeDecl', ['IdentifierType', ['int']]]]]],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])

        self.assertEqual(self.get_decl('int zz(int [restrict][5]);'),
            ['Decl', 'zz',
                ['FuncDecl',
                    [['Typename', ['ArrayDecl', '', ['restrict'],
                        ['ArrayDecl', '5', [],
                            ['TypeDecl', ['IdentifierType', ['int']]]]]]],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])

        self.assertEqual(self.get_decl('int zz(int [const restrict volatile 10][5]);'),
            ['Decl', 'zz',
                ['FuncDecl',
                    [['Typename', ['ArrayDecl', '10', ['const', 'restrict', 'volatile'],
                        ['ArrayDecl', '5', [],
                            ['TypeDecl', ['IdentifierType', ['int']]]]]]],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])

    def test_qualifiers_storage_specifiers(self):
        def assert_qs(txt, index, quals, storage):
            d = self.parse(txt).ext[index]
            self.assertEqual(d.quals, quals)
            self.assertEqual(d.storage, storage)

        assert_qs("extern int p;", 0, [], ['extern'])
        assert_qs("const long p = 6;", 0, ['const'], [])

        d1 = "static const int p, q, r;"
        for i in range(3):
            assert_qs(d1, i, ['const'], ['static'])

        d2 = "static char * const p;"
        assert_qs(d2, 0, [], ['static'])
        pdecl = self.parse(d2).ext[0].type
        self.assertIsInstance(pdecl, PtrDecl)
        self.assertEqual(pdecl.quals, ['const'])

    def test_sizeof(self):
        e = """
            void foo()
            {
                int a = sizeof k;
                int b = sizeof(int);
                int c = sizeof(int**);;

                char* p = "just to make sure this parses w/o error...";
                int d = sizeof(int());
            }
        """
        compound = self.parse(e).ext[0].body

        s1 = compound.block_items[0].init
        self.assertIsInstance(s1, UnaryOp)
        self.assertEqual(s1.op, 'sizeof')
        self.assertIsInstance(s1.expr, ID)
        self.assertEqual(s1.expr.name, 'k')

        s2 = compound.block_items[1].init
        self.assertEqual(expand_decl(s2.expr),
            ['Typename', ['TypeDecl', ['IdentifierType', ['int']]]])

        s3 = compound.block_items[2].init
        self.assertEqual(expand_decl(s3.expr),
            ['Typename',
                ['PtrDecl',
                    ['PtrDecl',
                        ['TypeDecl',
                            ['IdentifierType', ['int']]]]]])

    def test_offsetof(self):
        def expand_ref(n):
            if isinstance(n, StructRef):
                return ['StructRef', expand_ref(n.name), expand_ref(n.field)]
            elif isinstance(n, ArrayRef):
                return ['ArrayRef', expand_ref(n.name), expand_ref(n.subscript)]
            elif isinstance(n, ID):
                return ['ID', n.name]
            elif isinstance(n, Constant):
                return ['Constant', n.type, n.value]
            else:
                raise TypeError("Unexpected type " + n.__class__.__name__)

        e = """
            void foo() {
                int a = offsetof(struct S, p);
                a.b = offsetof(struct sockaddr, sp) + strlen(bar);
                int a = offsetof(struct S, p.q.r);
                int a = offsetof(struct S, p[5].q[4][5]);
            }
            """
        compound = self.parse(e).ext[0].body
        s1 = compound.block_items[0].init
        self.assertIsInstance(s1, FuncCall)
        self.assertIsInstance(s1.name, ID)
        self.assertEqual(s1.name.name, 'offsetof')
        self.assertIsInstance(s1.args.exprs[0], Typename)
        self.assertIsInstance(s1.args.exprs[1], ID)
        s3 = compound.block_items[2].init
        self.assertIsInstance(s3.args.exprs[1], StructRef)
        self.assertEqual(expand_ref(s3.args.exprs[1]),
            ['StructRef',
                ['StructRef', ['ID', 'p'], ['ID', 'q']],
                ['ID', 'r']])
        s4 = compound.block_items[3].init
        self.assertIsInstance(s4.args.exprs[1], ArrayRef)
        self.assertEqual(expand_ref(s4.args.exprs[1]),
            ['ArrayRef',
                ['ArrayRef',
                    ['StructRef',
                        ['ArrayRef', ['ID', 'p'], ['Constant', 'int', '5']],
                        ['ID', 'q']],
                    ['Constant', 'int', '4']],
                ['Constant', 'int', '5']])

    def test_compound_statement(self):
        e = """
            void foo() {
            }
            """
        compound = self.parse(e).ext[0].body
        self.assertIsInstance(compound, Compound)
        self.assert_coord(compound, 2)

    # The C99 compound literal feature
    #
    def test_compound_literals(self):
        ps1 = self.parse(r'''
            void foo() {
                p = (long long){k};
                tc = (struct jk){.a = {1, 2}, .b[0] = t};
            }''')

        compound = ps1.ext[0].body.block_items[0].rvalue
        self.assertEqual(expand_decl(compound.type),
            ['Typename', ['TypeDecl', ['IdentifierType', ['long', 'long']]]])
        self.assertEqual(expand_init(compound.init),
            [['ID', 'k']])

        compound = ps1.ext[0].body.block_items[1].rvalue
        self.assertEqual(expand_decl(compound.type),
            ['Typename', ['TypeDecl', ['Struct', 'jk', []]]])
        self.assertEqual(expand_init(compound.init),
            [
                ([['ID', 'a']], [['Constant', 'int', '1'], ['Constant', 'int', '2']]),
                ([['ID', 'b'], ['Constant', 'int', '0']], ['ID', 't'])])

    def test_enums(self):
        e1 = "enum mycolor op;"
        e1_type = self.parse(e1).ext[0].type.type

        self.assertIsInstance(e1_type, Enum)
        self.assertEqual(e1_type.name, 'mycolor')
        self.assertEqual(e1_type.values, None)

        e2 = "enum mysize {large=20, small, medium} shoes;"
        e2_type = self.parse(e2).ext[0].type.type

        self.assertIsInstance(e2_type, Enum)
        self.assertEqual(e2_type.name, 'mysize')

        e2_elist = e2_type.values
        self.assertIsInstance(e2_elist, EnumeratorList)

        for e2_eval in e2_elist.enumerators:
            self.assertIsInstance(e2_eval, Enumerator)

        self.assertEqual(e2_elist.enumerators[0].name, 'large')
        self.assertEqual(e2_elist.enumerators[0].value.value, '20')
        self.assertEqual(e2_elist.enumerators[2].name, 'medium')
        self.assertEqual(e2_elist.enumerators[2].value, None)

        # enum with trailing comma (C99 feature)
        e3 = """
            enum
            {
                red,
                blue,
                green,
            } color;
            """

        e3_type = self.parse(e3).ext[0].type.type
        self.assertIsInstance(e3_type, Enum)
        e3_elist = e3_type.values
        self.assertIsInstance(e3_elist, EnumeratorList)

        for e3_eval in e3_elist.enumerators:
            self.assertIsInstance(e3_eval, Enumerator)

        self.assertEqual(e3_elist.enumerators[0].name, 'red')
        self.assertEqual(e3_elist.enumerators[0].value, None)
        self.assertEqual(e3_elist.enumerators[1].name, 'blue')
        self.assertEqual(e3_elist.enumerators[2].name, 'green')

    def test_typedef(self):
        # without typedef, error
        s1 = """
            node k;
        """
        self.assertRaises(ParseError, self.parse, s1)

        # now with typedef, works
        s2 = """
            typedef void* node;
            node k;
        """
        ps2 = self.parse(s2)
        self.assertEqual(expand_decl(ps2.ext[0]),
            ['Typedef', 'node',
                ['PtrDecl',
                    ['TypeDecl', ['IdentifierType', ['void']]]]])

        self.assertEqual(expand_decl(ps2.ext[1]),
            ['Decl', 'k',
                ['TypeDecl', ['IdentifierType', ['node']]]])

        s3 = """
            typedef int T;
            typedef T *pT;

            pT aa, bb;
        """
        ps3 = self.parse(s3)
        self.assertEqual(expand_decl(ps3.ext[3]),
            ['Decl', 'bb',
                ['TypeDecl', ['IdentifierType', ['pT']]]])

        s4 = '''
            typedef char* __builtin_va_list;
            typedef __builtin_va_list __gnuc_va_list;
        '''
        ps4 = self.parse(s4)
        self.assertEqual(expand_decl(ps4.ext[1]),
            ['Typedef', '__gnuc_va_list',
                ['TypeDecl',
                    ['IdentifierType', ['__builtin_va_list']]]])

        s5 = '''typedef struct tagHash Hash;'''
        ps5 = self.parse(s5)
        self.assertEqual(expand_decl(ps5.ext[0]),
            ['Typedef', 'Hash', ['TypeDecl', ['Struct', 'tagHash', []]]])

    def test_struct_union(self):
        s1 = """
            struct {
                int id;
                char* name;
            } joe;
            """

        self.assertEqual(expand_decl(self.parse(s1).ext[0]),
            ['Decl', 'joe',
                ['TypeDecl', ['Struct', None,
                    [   ['Decl', 'id',
                            ['TypeDecl',
                                ['IdentifierType', ['int']]]],
                        ['Decl', 'name',
                            ['PtrDecl',
                                ['TypeDecl',
                                    ['IdentifierType', ['char']]]]]]]]])

        s2 = """
            struct node p;
        """
        self.assertEqual(expand_decl(self.parse(s2).ext[0]),
            ['Decl', 'p',
                ['TypeDecl', ['Struct', 'node', []]]])

        s21 = """
            union pri ra;
        """
        self.assertEqual(expand_decl(self.parse(s21).ext[0]),
            ['Decl', 'ra',
                ['TypeDecl', ['Union', 'pri', []]]])

        s3 = """
            struct node* p;
        """
        self.assertEqual(expand_decl(self.parse(s3).ext[0]),
            ['Decl', 'p',
                ['PtrDecl',
                    ['TypeDecl', ['Struct', 'node', []]]]])

        s4 = """
            struct node;
        """
        self.assertEqual(expand_decl(self.parse(s4).ext[0]),
            ['Decl', None,
                ['Struct', 'node', []]])

        s5 = """
            union
            {
                struct
                {
                    int type;
                } n;

                struct
                {
                    int type;
                    int intnode;
                } ni;
            } u;
        """
        self.assertEqual(expand_decl(self.parse(s5).ext[0]),
            ['Decl', 'u',
                ['TypeDecl',
                    ['Union', None,
                        [['Decl', 'n',
                            ['TypeDecl',
                                ['Struct', None,
                                    [['Decl', 'type',
                                        ['TypeDecl', ['IdentifierType', ['int']]]]]]]],
                        ['Decl', 'ni',
                            ['TypeDecl',
                                ['Struct', None,
                                    [['Decl', 'type',
                                        ['TypeDecl', ['IdentifierType', ['int']]]],
                                    ['Decl', 'intnode',
                                        ['TypeDecl', ['IdentifierType', ['int']]]]]]]]]]]])

        s6 = """
            typedef struct foo_tag
            {
                void* data;
            } foo, *pfoo;
        """
        s6_ast = self.parse(s6)

        self.assertEqual(expand_decl(s6_ast.ext[0]),
            ['Typedef', 'foo',
                ['TypeDecl',
                    ['Struct', 'foo_tag',
                        [['Decl', 'data',
                            ['PtrDecl', ['TypeDecl', ['IdentifierType', ['void']]]]]]]]])

        self.assertEqual(expand_decl(s6_ast.ext[1]),
            ['Typedef', 'pfoo',
                ['PtrDecl',
                    ['TypeDecl',
                        ['Struct', 'foo_tag',
                            [['Decl', 'data',
                                ['PtrDecl', ['TypeDecl', ['IdentifierType', ['void']]]]]]]]]])

        s7 = r"""
            struct _on_exit_args {
                void *  _fnargs[32];
                void *  _dso_handle[32];

                long _fntypes;
                #line 77 "D:\eli\cpp_stuff\libc_include/sys/reent.h"

                long _is_cxa;
            };
        """

        s7_ast = self.parse(s7, filename='test.c')
        self.assert_coord(s7_ast.ext[0].type.decls[2], 6, 22, 'test.c')
        self.assert_coord(s7_ast.ext[0].type.decls[3], 78, 22,
            r'D:\eli\cpp_stuff\libc_include/sys/reent.h')

        s8 = """
            typedef enum tagReturnCode {SUCCESS, FAIL} ReturnCode;

            typedef struct tagEntry
            {
                char* key;
                char* value;
            } Entry;


            typedef struct tagNode
            {
                Entry* entry;

                struct tagNode* next;
            } Node;

            typedef struct tagHash
            {
                unsigned int table_size;

                Node** heads;

            } Hash;
        """
        s8_ast = self.parse(s8)
        self.assertEqual(expand_decl(s8_ast.ext[3]),
            ['Typedef', 'Hash',
                ['TypeDecl', ['Struct', 'tagHash',
                    [['Decl', 'table_size',
                        ['TypeDecl', ['IdentifierType', ['unsigned', 'int']]]],
                    ['Decl', 'heads',
                        ['PtrDecl', ['PtrDecl', ['TypeDecl', ['IdentifierType', ['Node']]]]]]]]]])

    def test_struct_enum(self):
        s1 = """
            struct Foo {
                enum Bar { A = 1 };
            };
        """
        self.assertEqual(expand_decl(self.parse(s1).ext[0]),
            ['Decl', None,
             ['Struct', 'Foo',
              [['Decl', None,
                ['Enum', 'Bar', ['A']]]]]])
        s2 = """
            struct Foo {
                enum Bar { A = 1, B, C } bar;
                enum Baz { D = A } baz;
            } foo;
        """
        self.assertEqual(expand_decl(self.parse(s2).ext[0]),
            ['Decl', 'foo',
             ['TypeDecl', ['Struct', 'Foo',
              [['Decl', 'bar',
                ['TypeDecl', ['Enum', 'Bar', ['A', 'B', 'C']]]],
               ['Decl', 'baz',
                ['TypeDecl', ['Enum', 'Baz', ['D']]]]]]]])

    def test_struct_with_extra_semis_inside(self):
        s1 = """
            struct {
                int a;;
            } foo;
        """
        s1_ast = self.parse(s1)
        self.assertEqual(expand_decl(s1_ast.ext[0]),
            ['Decl', 'foo',
                ['TypeDecl', ['Struct', None,
                    [['Decl', 'a',
                        ['TypeDecl', ['IdentifierType', ['int']]]]]]]])

        s2 = """
            struct {
                int a;;;;
                float b, c;
                ;;
                char d;
            } foo;
        """
        s2_ast = self.parse(s2)
        self.assertEqual(expand_decl(s2_ast.ext[0]),
            ['Decl', 'foo',
                ['TypeDecl', ['Struct', None,
                   [['Decl', 'a', ['TypeDecl', ['IdentifierType', ['int']]]],
                    ['Decl', 'b', ['TypeDecl', ['IdentifierType', ['float']]]],
                    ['Decl', 'c', ['TypeDecl', ['IdentifierType', ['float']]]],
                    ['Decl', 'd',
                        ['TypeDecl', ['IdentifierType', ['char']]]]]]]])

    def test_struct_with_initial_semi(self):
        s1 = """
            struct {
                ;int a;
            } foo;
        """
        s1_ast = self.parse(s1)
        self.assertEqual(expand_decl(s1_ast.ext[0]),
            ['Decl', 'foo',
                ['TypeDecl', ['Struct', None,
                    [['Decl', 'a',
                        ['TypeDecl', ['IdentifierType', ['int']]]]]]]])

    def test_anonymous_struct_union(self):
        s1 = """
            union
            {
                union
                {
                    int i;
                    long l;
                };

                struct
                {
                    int type;
                    int intnode;
                };
            } u;
        """

        self.assertEqual(expand_decl(self.parse(s1).ext[0]),
            ['Decl', 'u',
                ['TypeDecl',
                    ['Union', None,
                        [['Decl', None,
                            ['Union', None,
                                [['Decl', 'i',
                                    ['TypeDecl',
                                        ['IdentifierType', ['int']]]],
                                ['Decl', 'l',
                                    ['TypeDecl',
                                        ['IdentifierType', ['long']]]]]]],
                        ['Decl', None,
                            ['Struct', None,
                                [['Decl', 'type',
                                    ['TypeDecl',
                                        ['IdentifierType', ['int']]]],
                                ['Decl', 'intnode',
                                    ['TypeDecl',
                                        ['IdentifierType', ['int']]]]]]]]]]])

        s2 = """
            struct
            {
                int i;
                union
                {
                    int id;
                    char* name;
                };
                float f;
            } joe;
            """

        self.assertEqual(expand_decl(self.parse(s2).ext[0]),
            ['Decl', 'joe',
                ['TypeDecl',
                    ['Struct', None,
                       [['Decl', 'i',
                            ['TypeDecl',
                                ['IdentifierType', ['int']]]],
                        ['Decl', None,
                            ['Union', None,
                                [['Decl', 'id',
                                    ['TypeDecl',
                                        ['IdentifierType', ['int']]]],
                                ['Decl', 'name',
                                    ['PtrDecl',
                                        ['TypeDecl',
                                            ['IdentifierType', ['char']]]]]]]],
                        ['Decl', 'f',
                            ['TypeDecl',
                                ['IdentifierType', ['float']]]]]]]])

        # ISO/IEC 9899:201x Commitee Draft 2010-11-16, N1539
        # section 6.7.2.1, par. 19, example 1
        s3 = """
            struct v {
                union {
                    struct { int i, j; };
                    struct { long k, l; } w;
                };
                int m;
            } v1;
            """

        self.assertEqual(expand_decl(self.parse(s3).ext[0]),
            ['Decl', 'v1',
                ['TypeDecl',
                    ['Struct', 'v',
                       [['Decl', None,
                            ['Union', None,
                                [['Decl', None,
                                    ['Struct', None,
                                        [['Decl', 'i',
                                            ['TypeDecl',
                                                ['IdentifierType', ['int']]]],
                                        ['Decl', 'j',
                                            ['TypeDecl',
                                                ['IdentifierType', ['int']]]]]]],
                                ['Decl', 'w',
                                    ['TypeDecl',
                                        ['Struct', None,
                                            [['Decl', 'k',
                                                ['TypeDecl',
                                                    ['IdentifierType', ['long']]]],
                                            ['Decl', 'l',
                                                ['TypeDecl',
                                                    ['IdentifierType', ['long']]]]]]]]]]],
                        ['Decl', 'm',
                            ['TypeDecl',
                                ['IdentifierType', ['int']]]]]]]])

        s4 = """
            struct v {
                int i;
                float;
            } v2;"""
        # just make sure this doesn't raise ParseError
        self.parse(s4)

    def test_struct_members_namespace(self):
        """ Tests that structure/union member names reside in a separate
            namespace and can be named after existing types.
        """
        s1 = """
                typedef int Name;
                typedef Name NameArray[10];

                struct {
                    Name Name;
                    Name NameArray[3];
                } sye;

                void main(void)
                {
                    sye.Name = 1;
                }
            """

        s1_ast = self.parse(s1)
        self.assertEqual(expand_decl(s1_ast.ext[2]),
            ['Decl', 'sye',
                ['TypeDecl', ['Struct', None,
                    [   ['Decl', 'Name',
                            ['TypeDecl',
                                ['IdentifierType', ['Name']]]],
                        ['Decl', 'NameArray',
                            ['ArrayDecl', '3', [],
                                ['TypeDecl', ['IdentifierType', ['Name']]]]]]]]])
        self.assertEqual(s1_ast.ext[3].body.block_items[0].lvalue.field.name, 'Name')

    def test_struct_bitfields(self):
        # a struct with two bitfields, one unnamed
        s1 = """
            struct {
                int k:6;
                int :2;
            } joe;
            """

        parsed_struct = self.parse(s1).ext[0]

        # We can see here the name of the decl for the unnamed bitfield is
        # None, but expand_decl doesn't show bitfield widths
        # ...
        self.assertEqual(expand_decl(parsed_struct),
            ['Decl', 'joe',
                ['TypeDecl', ['Struct', None,
                    [   ['Decl', 'k',
                            ['TypeDecl',
                                ['IdentifierType', ['int']]]],
                        ['Decl', None,
                            ['TypeDecl',
                                ['IdentifierType', ['int']]]]]]]])

        # ...
        # so we test them manually
        self.assertEqual(parsed_struct.type.type.decls[0].bitsize.value, '6')
        self.assertEqual(parsed_struct.type.type.decls[1].bitsize.value, '2')

    def test_struct_empty(self):
        """
        Tests that parsing an empty struct works.

        Empty structs do NOT follow C99 (See 6.2.5-20 of the C99 standard).
        This is nevertheless supported by some compilers (clang, gcc),
        especially when using FORTIFY code.
        Some compilers (visual) will fail to compile with an error.
        """
        # an empty struct. This is NOT C99 compliant
        s1 = """
                 struct foo { };
             """

        parsed_struct = self.parse(s1).ext[0]
        self.assertEqual(expand_decl(parsed_struct),
                         ['Decl', None, ['Struct', 'foo', []]])

        s2 = """struct { } foo;"""
        parsed_struct = self.parse(s2).ext[0]
        self.assertEqual(expand_decl(parsed_struct),
                         ['Decl', 'foo', ['TypeDecl', ['Struct', None, []]]])

        s3 = """union { } foo;"""
        parsed_struct = self.parse(s3).ext[0]
        self.assertEqual(expand_decl(parsed_struct),
                         ['Decl', 'foo', ['TypeDecl', ['Union', None, []]]])

    def test_tags_namespace(self):
        """ Tests that the tags of structs/unions/enums reside in a separate namespace and
            can be named after existing types.
        """
        s1 = """
                typedef int tagEntry;

                struct tagEntry
                {
                    char* key;
                    char* value;
                } Entry;
            """

        s1_ast = self.parse(s1)
        self.assertEqual(expand_decl(s1_ast.ext[1]),
            ['Decl', 'Entry',
                ['TypeDecl', ['Struct', 'tagEntry',
                    [['Decl', 'key',
                        ['PtrDecl', ['TypeDecl', ['IdentifierType', ['char']]]]],
                    ['Decl', 'value',
                        ['PtrDecl', ['TypeDecl', ['IdentifierType', ['char']]]]]]]]])

        s2 = """
                struct tagEntry;

                typedef struct tagEntry tagEntry;

                struct tagEntry
                {
                    char* key;
                    char* value;
                } Entry;
            """

        s2_ast = self.parse(s2)
        self.assertEqual(expand_decl(s2_ast.ext[2]),
            ['Decl', 'Entry',
                ['TypeDecl', ['Struct', 'tagEntry',
                    [['Decl', 'key',
                        ['PtrDecl', ['TypeDecl', ['IdentifierType', ['char']]]]],
                    ['Decl', 'value',
                        ['PtrDecl', ['TypeDecl', ['IdentifierType', ['char']]]]]]]]])

        s3 = """
                typedef int mytag;

                enum mytag {ABC, CDE};
                enum mytag joe;
            """

        s3_type = self.parse(s3).ext[1].type

        self.assertIsInstance(s3_type, Enum)
        self.assertEqual(s3_type.name, 'mytag')

    def test_multi_decls(self):
        d1 = 'int a, b;'

        self.assertEqual(self.get_decl(d1, 0),
            ['Decl', 'a', ['TypeDecl', ['IdentifierType', ['int']]]])
        self.assertEqual(self.get_decl(d1, 1),
            ['Decl', 'b', ['TypeDecl', ['IdentifierType', ['int']]]])

        d2 = 'char* p, notp, ar[4];'
        self.assertEqual(self.get_decl(d2, 0),
            ['Decl', 'p',
                ['PtrDecl',
                    ['TypeDecl', ['IdentifierType', ['char']]]]])
        self.assertEqual(self.get_decl(d2, 1),
            ['Decl', 'notp', ['TypeDecl', ['IdentifierType', ['char']]]])
        self.assertEqual(self.get_decl(d2, 2),
            ['Decl', 'ar',
                ['ArrayDecl', '4', [],
                    ['TypeDecl', ['IdentifierType', ['char']]]]])

    def test_invalid_multiple_types_error(self):
        bad = [
            'int enum {ab, cd} fubr;',
            'enum kid char brbr;']

        for b in bad:
            self.assertRaises(ParseError, self.parse, b)

    def test_invalid_typedef_storage_qual_error(self):
        """ Tests that using typedef as a storage qualifier is correctly flagged
            as an error.
        """
        bad = 'typedef const int foo(int a) { return 0; }'
        self.assertRaises(ParseError, self.parse, bad)

    def test_duplicate_typedef(self):
        """ Tests that redeclarations of existing types are parsed correctly.
            This is non-standard, but allowed by many compilers.
        """
        d1 = '''
            typedef int numbertype;
            typedef int numbertype;
        '''

        self.assertEqual(self.get_decl(d1, 0),
            ['Typedef', 'numbertype',
                ['TypeDecl', ['IdentifierType', ['int']]]])
        self.assertEqual(self.get_decl(d1, 1),
            ['Typedef', 'numbertype',
                ['TypeDecl', ['IdentifierType', ['int']]]])

        d2 = '''
            typedef int (*funcptr)(int x);
            typedef int (*funcptr)(int x);
        '''
        self.assertEqual(self.get_decl(d2, 0),
            ['Typedef', 'funcptr',
                ['PtrDecl', ['FuncDecl',
                    [['Decl', 'x', ['TypeDecl', ['IdentifierType', ['int']]]]],
                    ['TypeDecl', ['IdentifierType', ['int']]]]]])
        self.assertEqual(self.get_decl(d2, 1),
            ['Typedef', 'funcptr',
                ['PtrDecl', ['FuncDecl',
                    [['Decl', 'x', ['TypeDecl', ['IdentifierType', ['int']]]]],
                    ['TypeDecl', ['IdentifierType', ['int']]]]]])

        d3 = '''
            typedef int numberarray[5];
            typedef int numberarray[5];
        '''
        self.assertEqual(self.get_decl(d3, 0),
            ['Typedef', 'numberarray',
                ['ArrayDecl', '5', [],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])
        self.assertEqual(self.get_decl(d3, 1),
            ['Typedef', 'numberarray',
                ['ArrayDecl', '5', [],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])

    def test_decl_inits(self):
        d1 = 'int a = 16;'
        #~ self.parse(d1).show()
        self.assertEqual(self.get_decl(d1),
            ['Decl', 'a', ['TypeDecl', ['IdentifierType', ['int']]]])
        self.assertEqual(self.get_decl_init(d1),
            ['Constant', 'int', '16'])

        d1_1 = 'float f = 0xEF.56p1;'
        self.assertEqual(self.get_decl_init(d1_1),
            ['Constant', 'float', '0xEF.56p1'])

        d1_2 = 'int bitmask = 0b1001010;'
        self.assertEqual(self.get_decl_init(d1_2),
            ['Constant', 'int', '0b1001010'])

        d2 = 'long ar[] = {7, 8, 9};'
        self.assertEqual(self.get_decl(d2),
            ['Decl', 'ar',
                ['ArrayDecl', '', [],
                    ['TypeDecl', ['IdentifierType', ['long']]]]])
        self.assertEqual(self.get_decl_init(d2),
            [   ['Constant', 'int', '7'],
                ['Constant', 'int', '8'],
                ['Constant', 'int', '9']])

        d21 = 'long ar[4] = {};'
        self.assertEqual(self.get_decl_init(d21), [])

        d3 = 'char p = j;'
        self.assertEqual(self.get_decl(d3),
            ['Decl', 'p', ['TypeDecl', ['IdentifierType', ['char']]]])
        self.assertEqual(self.get_decl_init(d3),
            ['ID', 'j'])

        d4 = "char x = 'c', *p = {0, 1, 2, {4, 5}, 6};"
        self.assertEqual(self.get_decl(d4, 0),
            ['Decl', 'x', ['TypeDecl', ['IdentifierType', ['char']]]])
        self.assertEqual(self.get_decl_init(d4, 0),
            ['Constant', 'char', "'c'"])
        self.assertEqual(self.get_decl(d4, 1),
            ['Decl', 'p',
                ['PtrDecl',
                    ['TypeDecl', ['IdentifierType', ['char']]]]])

        self.assertEqual(self.get_decl_init(d4, 1),
            [   ['Constant', 'int', '0'],
                ['Constant', 'int', '1'],
                ['Constant', 'int', '2'],
                [['Constant', 'int', '4'],
                 ['Constant', 'int', '5']],
                ['Constant', 'int', '6']])

        d5 = 'float d = 1.0;'
        self.assertEqual(self.get_decl_init(d5),
            ['Constant', 'double', '1.0'])

        d51 = 'float ld = 1.0l;'
        self.assertEqual(self.get_decl_init(d51),
            ['Constant', 'long double', '1.0l'])

        d52 = 'float ld = 1.0L;'
        self.assertEqual(self.get_decl_init(d52),
            ['Constant', 'long double', '1.0L'])

        d53 = 'float ld = 1.0f;'
        self.assertEqual(self.get_decl_init(d53),
            ['Constant', 'float', '1.0f'])

        d54 = 'float ld = 1.0F;'
        self.assertEqual(self.get_decl_init(d54),
            ['Constant', 'float', '1.0F'])

        d55 = 'float ld = 0xDE.38p0;'
        self.assertEqual(self.get_decl_init(d55),
            ['Constant', 'float', '0xDE.38p0'])

        d6 = 'int i = 1;'
        self.assertEqual(self.get_decl_init(d6),
            ['Constant', 'int', '1'])

        d61 = 'long int li = 1l;'
        self.assertEqual(self.get_decl_init(d61),
            ['Constant', 'long int', '1l'])

        d62 = 'unsigned int ui = 1u;'
        self.assertEqual(self.get_decl_init(d62),
            ['Constant', 'unsigned int', '1u'])

        d63 = 'unsigned long long int ulli = 1LLU;'
        self.assertEqual(self.get_decl_init(d63),
            ['Constant', 'unsigned long long int', '1LLU'])

    def test_decl_named_inits(self):
        d1 = 'int a = {.k = 16};'
        self.assertEqual(self.get_decl_init(d1),
            [(   [['ID', 'k']],
                 ['Constant', 'int', '16'])])

        d2 = 'int a = { [0].a = {1}, [1].a[0] = 2 };'
        self.assertEqual(self.get_decl_init(d2),
            [
                ([['Constant', 'int', '0'], ['ID', 'a']],
                    [['Constant', 'int', '1']]),
                ([['Constant', 'int', '1'], ['ID', 'a'], ['Constant', 'int', '0']],
                    ['Constant', 'int', '2'])])

        d3 = 'int a = { .a = 1, .c = 3, 4, .b = 5};'
        self.assertEqual(self.get_decl_init(d3),
            [
                ([['ID', 'a']], ['Constant', 'int', '1']),
                ([['ID', 'c']], ['Constant', 'int', '3']),
                ['Constant', 'int', '4'],
                ([['ID', 'b']], ['Constant', 'int', '5'])])

    def test_function_definitions(self):
        def parse_fdef(str):
            return self.parse(str).ext[0]

        def fdef_decl(fdef):
            return expand_decl(fdef.decl)

        f1 = parse_fdef('''
        int factorial(int p)
        {
            return 3;
        }
        ''')

        self.assertEqual(fdef_decl(f1),
            ['Decl', 'factorial',
                ['FuncDecl',
                    [['Decl', 'p', ['TypeDecl', ['IdentifierType', ['int']]]]],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])

        self.assertEqual(type(f1.body.block_items[0]), Return)

        f2 = parse_fdef('''
        char* zzz(int p, char* c)
        {
            int a;
            char b;

            a = b + 2;
            return 3;
        }
        ''')

        self.assertEqual(fdef_decl(f2),
            ['Decl', 'zzz',
                ['FuncDecl',
                    [   ['Decl', 'p', ['TypeDecl', ['IdentifierType', ['int']]]],
                        ['Decl', 'c', ['PtrDecl',
                                        ['TypeDecl', ['IdentifierType', ['char']]]]]],
                    ['PtrDecl', ['TypeDecl', ['IdentifierType', ['char']]]]]])

        self.assertEqual(list(map(type, f2.body.block_items)),
            [Decl, Decl, Assignment, Return])

        f3 = parse_fdef('''
        char* zzz(p, c)
        long p, *c;
        {
            int a;
            char b;

            a = b + 2;
            return 3;
        }
        ''')

        self.assertEqual(fdef_decl(f3),
            ['Decl', 'zzz',
                ['FuncDecl',
                    [   ['ID', 'p'],
                        ['ID', 'c']],
                    ['PtrDecl', ['TypeDecl', ['IdentifierType', ['char']]]]]])

        self.assertEqual(list(map(type, f3.body.block_items)),
            [Decl, Decl, Assignment, Return])

        self.assertEqual(expand_decl(f3.param_decls[0]),
            ['Decl', 'p', ['TypeDecl', ['IdentifierType', ['long']]]])
        self.assertEqual(expand_decl(f3.param_decls[1]),
            ['Decl', 'c', ['PtrDecl', ['TypeDecl', ['IdentifierType', ['long']]]]])

        # function return values and parameters may not have type information
        f4 = parse_fdef('''
        que(p)
        {
            return 3;
        }
        ''')

        self.assertEqual(fdef_decl(f4),
            ['Decl', 'que',
                ['FuncDecl',
                    [['ID', 'p']],
                    ['TypeDecl', ['IdentifierType', ['int']]]]])

    def test_unified_string_literals(self):
        # simple string, for reference
        d1 = self.get_decl_init('char* s = "hello";')
        self.assertEqual(d1, ['Constant', 'string', '"hello"'])

        d2 = self.get_decl_init('char* s = "hello" " world";')
        self.assertEqual(d2, ['Constant', 'string', '"hello world"'])

        # the test case from issue 6
        d3 = self.parse(r'''
            int main() {
                fprintf(stderr,
                "Wrong Params?\n"
                "Usage:\n"
                "%s <binary_file_path>\n",
                argv[0]
                );
            }
        ''')

        self.assertEqual(
            d3.ext[0].body.block_items[0].args.exprs[1].value,
            r'"Wrong Params?\nUsage:\n%s <binary_file_path>\n"')

        d4 = self.get_decl_init('char* s = "" "foobar";')
        self.assertEqual(d4, ['Constant', 'string', '"foobar"'])

        d5 = self.get_decl_init(r'char* s = "foo\"" "bar";')
        self.assertEqual(d5, ['Constant', 'string', r'"foo\"bar"'])

        # This is not correct based on the the C spec, but testing it here to
        # see the behavior in action. Will have to fix this
        # for https://github.com/eliben/pycparser/issues/392
        #
        # The spec says in section 6.4.5 that "escape sequences are converted
        # into single members of the execution character set just prior to
        # adjacent string literal concatenation".
        d6 = self.get_decl_init(r'char* s = "\1" "23";')
        self.assertEqual(d6, ['Constant', 'string', r'"\123"'])

    def test_unified_wstring_literals(self):
        d1 = self.get_decl_init('char* s = L"hello" L"world";')
        self.assertEqual(d1, ['Constant', 'string', 'L"helloworld"'])

        d2 = self.get_decl_init('char* s = L"hello " L"world" L" and I";')
        self.assertEqual(d2, ['Constant', 'string', 'L"hello world and I"'])

    def test_inline_specifier(self):
        ps2 = self.parse('static inline void inlinefoo(void);')
        self.assertEqual(ps2.ext[0].funcspec, ['inline'])

    # variable length array
    def test_vla(self):
        ps2 = self.parse(r'''
            int main() {
                int size;
                int var[size = 5];

                int var2[*];
            }
        ''')
        self.assertIsInstance(ps2.ext[0].body.block_items[1].type.dim, Assignment)
        self.assertIsInstance(ps2.ext[0].body.block_items[2].type.dim, ID)

    def test_pragma(self):
        s1 = r'''
            #pragma bar
            void main() {
                #pragma foo
                for(;;) {}
                #pragma baz
                {
                    int i = 0;
                }
                #pragma
            }
            struct s {
            #pragma baz
            } s;
            '''
        s1_ast = self.parse(s1)
        self.assertIsInstance(s1_ast.ext[0], Pragma)
        self.assertEqual(s1_ast.ext[0].string, 'bar')
        self.assertEqual(s1_ast.ext[0].coord.line, 2)

        self.assertIsInstance(s1_ast.ext[1].body.block_items[0], Pragma)
        self.assertEqual(s1_ast.ext[1].body.block_items[0].string, 'foo')
        self.assertEqual(s1_ast.ext[1].body.block_items[0].coord.line, 4)

        self.assertIsInstance(s1_ast.ext[1].body.block_items[2], Pragma)
        self.assertEqual(s1_ast.ext[1].body.block_items[2].string, 'baz')
        self.assertEqual(s1_ast.ext[1].body.block_items[2].coord.line, 6)

        self.assertIsInstance(s1_ast.ext[1].body.block_items[4], Pragma)
        self.assertEqual(s1_ast.ext[1].body.block_items[4].string, '')
        self.assertEqual(s1_ast.ext[1].body.block_items[4].coord.line, 10)

        self.assertIsInstance(s1_ast.ext[2].type.type.decls[0], Pragma)
        self.assertEqual(s1_ast.ext[2].type.type.decls[0].string, 'baz')
        self.assertEqual(s1_ast.ext[2].type.type.decls[0].coord.line, 13)

    def test_pragmacomp_or_statement(self):
        s1 = r'''
            void main() {
                int sum = 0;
                for (int i; i < 3; i++)
                    #pragma omp critical
                    sum += 1;

                while(sum < 10)
                    #pragma omp critical
                    sum += 1;

                mylabel:
                    #pragma foo
                    sum += 10;

                if (sum > 10)
                    #pragma bar
                    sum = 10;

                switch (sum)
                case 10:
                    #pragma foo
                    sum = 20;
            }
        '''
        s1_ast = self.parse(s1)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[1], For)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[1].stmt, Compound)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[1].stmt.block_items[0], Pragma)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[1].stmt.block_items[1], Assignment)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[2], While)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[2].stmt, Compound)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[2].stmt.block_items[0], Pragma)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[2].stmt.block_items[1], Assignment)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[3], Label)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[3].stmt, Compound)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[3].stmt.block_items[0], Pragma)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[3].stmt.block_items[1], Assignment)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[4], If)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[4].iftrue, Compound)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[4].iftrue.block_items[0], Pragma)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[4].iftrue.block_items[1], Assignment)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[5], Switch)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[5].stmt.stmts[0], Compound)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[5].stmt.stmts[0].block_items[0],
                              Pragma)
        self.assertIsInstance(s1_ast.ext[0].body.block_items[5].stmt.stmts[0].block_items[1],
                              Assignment)


class TestCParser_whole_code(TestCParser_base):
    """ Testing of parsing whole chunks of code.

        Since I don't want to rely on the structure of ASTs too
        much, most of these tests are implemented with visitors.
    """
    # A simple helper visitor that lists the values of all the
    # Constant nodes it sees.
    #
    class ConstantVisitor(NodeVisitor):
        def __init__(self):
            self.values = []

        def visit_Constant(self, node):
            self.values.append(node.value)

    # This visitor counts the amount of references to the ID
    # with the name provided to it in the constructor.
    #
    class IDNameCounter(NodeVisitor):
        def __init__(self, name):
            self.name = name
            self.nrefs = 0

        def visit_ID(self, node):
            if node.name == self.name:
                self.nrefs += 1

    # Counts the amount of nodes of a given class
    #
    class NodeKlassCounter(NodeVisitor):
        def __init__(self, node_klass):
            self.klass = node_klass
            self.n = 0

        def generic_visit(self, node):
            if node.__class__ == self.klass:
                self.n += 1

            NodeVisitor.generic_visit(self, node)

    def assert_all_Constants(self, code, constants):
        """ Asserts that the list of all Constant values (by
            'preorder' appearance) in the chunk of code is as
            given.
        """
        if isinstance(code, str):
            parsed = self.parse(code)
        else:
            parsed = code

        cv = self.ConstantVisitor()
        cv.visit(parsed)
        self.assertEqual(cv.values, constants)

    def assert_num_ID_refs(self, code, name, num):
        """ Asserts the number of references to the ID with
            the given name.
        """
        if isinstance(code, str):
            parsed = self.parse(code)
        else:
            parsed = code

        iv = self.IDNameCounter(name)
        iv.visit(parsed)
        self.assertEqual(iv.nrefs, num)

    def assert_num_klass_nodes(self, code, klass, num):
        """ Asserts the amount of klass nodes in the code.
        """
        if isinstance(code, str):
            parsed = self.parse(code)
        else:
            parsed = code

        cv = self.NodeKlassCounter(klass)
        cv.visit(parsed)
        self.assertEqual(cv.n, num)

    def test_expressions(self):
        e1 = '''int k = (r + 10.0) >> 6 + 8 << (3 & 0x14);'''
        self.assert_all_Constants(e1, ['10.0', '6', '8', '3', '0x14'])

        e2 = r'''char n = '\n', *prefix = "st_";'''
        self.assert_all_Constants(e2, [r"'\n'", '"st_"'])

        s1 = r'''int main() {
                    int i = 5, j = 6, k = 1;
                    if ((i=j && k == 1) || k > j)
                        printf("Hello, world\n");
                    return 0;
                 }'''
        ps1 = self.parse(s1)
        self.assert_all_Constants(ps1,
            ['5', '6', '1', '1', '"Hello, world\\n"', '0'])
        self.assert_num_ID_refs(ps1, 'i', 1)
        self.assert_num_ID_refs(ps1, 'j', 2)


    def test_statements(self):
        s1 = r'''
            void foo(){
            if (sp == 1)
                if (optind >= argc ||
                    argv[optind][0] != '-' || argv[optind][1] == '\0')
                        return -1;
                else if (strcmp(argv[optind], "--") == 0) {
                    optind++;
                    return -1;
                }
            }
        '''

        self.assert_all_Constants(s1,
            ['1', '0', r"'-'", '1', r"'\0'", '1', r'"--"', '0', '1'])

        ps1 = self.parse(s1)
        self.assert_num_ID_refs(ps1, 'argv', 3)
        self.assert_num_ID_refs(ps1, 'optind', 5)

        self.assert_num_klass_nodes(ps1, If, 3)
        self.assert_num_klass_nodes(ps1, Return, 2)
        self.assert_num_klass_nodes(ps1, FuncCall, 1) # strcmp
        self.assert_num_klass_nodes(ps1, BinaryOp, 7)

        # In the following code, Hash and Node were defined as
        # int to pacify the parser that sees they're used as
        # types
        #
        s2 = r'''
        typedef int Hash, Node;

        void HashDestroy(Hash* hash)
        {
            unsigned int i;

            if (hash == NULL)
                return;

            for (i = 0; i < hash->table_size; ++i)
            {
                Node* temp = hash->heads[i];

                while (temp != NULL)
                {
                    Node* temp2 = temp;

                    free(temp->entry->key);
                    free(temp->entry->value);
                    free(temp->entry);

                    temp = temp->next;

                    free(temp2);
                }
            }

            free(hash->heads);
            hash->heads = NULL;

            free(hash);
        }
        '''

        ps2 = self.parse(s2)
        self.assert_num_klass_nodes(ps2, FuncCall, 6)
        self.assert_num_klass_nodes(ps2, FuncDef, 1)
        self.assert_num_klass_nodes(ps2, For, 1)
        self.assert_num_klass_nodes(ps2, While, 1)
        self.assert_num_klass_nodes(ps2, StructRef, 10)

        # declarations don't count
        self.assert_num_ID_refs(ps2, 'hash', 6)
        self.assert_num_ID_refs(ps2, 'i', 4)

        s3 = r'''
        void x(void) {
          int a, b;
          if (a < b)
            do {
              a = 0;
            } while (0);
          else if (a == b) {
            a = 1;
          }
        }
        '''

        ps3 = self.parse(s3)
        self.assert_num_klass_nodes(ps3, DoWhile, 1)
        self.assert_num_ID_refs(ps3, 'a', 4)
        self.assert_all_Constants(ps3, ['0', '0', '1'])

    def test_empty_statements(self):
        s1 = r'''
        void foo(void){
            ;
            return;;

            ;
        }
        '''
        ps1 = self.parse(s1)
        self.assert_num_klass_nodes(ps1, EmptyStatement, 3)
        self.assert_num_klass_nodes(ps1, Return, 1)
        self.assert_coord(ps1.ext[0].body.block_items[0], 3)
        self.assert_coord(ps1.ext[0].body.block_items[1], 4)
        self.assert_coord(ps1.ext[0].body.block_items[2], 4)
        self.assert_coord(ps1.ext[0].body.block_items[3], 6)

    def test_switch_statement(self):
        def assert_case_node(node, const_value):
            self.assertIsInstance(node, Case)
            self.assertIsInstance(node.expr, Constant)
            self.assertEqual(node.expr.value, const_value)

        def assert_default_node(node):
            self.assertIsInstance(node, Default)

        s1 = r'''
        int foo(void) {
            switch (myvar) {
                case 10:
                    k = 10;
                    p = k + 1;
                    return 10;
                case 20:
                case 30:
                    return 20;
                default:
                    break;
            }
            return 0;
        }
        '''
        ps1 = self.parse(s1)
        switch = ps1.ext[0].body.block_items[0]

        block = switch.stmt.block_items
        self.assertEqual(len(block), 4)
        assert_case_node(block[0], '10')
        self.assertEqual(len(block[0].stmts), 3)
        assert_case_node(block[1], '20')
        self.assertEqual(len(block[1].stmts), 0)
        assert_case_node(block[2], '30')
        self.assertEqual(len(block[2].stmts), 1)
        assert_default_node(block[3])

        s2 = r'''
        int foo(void) {
            switch (myvar) {
                default:
                    joe = moe;
                    return 10;
                case 10:
                case 20:
                case 30:
                case 40:
                    break;
            }
            return 0;
        }
        '''
        ps2 = self.parse(s2)
        switch = ps2.ext[0].body.block_items[0]

        block = switch.stmt.block_items
        self.assertEqual(len(block), 5)
        assert_default_node(block[0])
        self.assertEqual(len(block[0].stmts), 2)
        assert_case_node(block[1], '10')
        self.assertEqual(len(block[1].stmts), 0)
        assert_case_node(block[2], '20')
        self.assertEqual(len(block[1].stmts), 0)
        assert_case_node(block[3], '30')
        self.assertEqual(len(block[1].stmts), 0)
        assert_case_node(block[4], '40')
        self.assertEqual(len(block[4].stmts), 1)

        s3 = r'''
        int foo(void) {
            switch (myvar) {
            }
            return 0;
        }
        '''
        ps3 = self.parse(s3)
        switch = ps3.ext[0].body.block_items[0]

        self.assertEqual(switch.stmt.block_items, [])

    def test_for_statement(self):
        s2 = r'''
        void x(void)
        {
            int i;
            for (i = 0; i < 5; ++i) {
                x = 50;
            }
        }
        '''
        ps2 = self.parse(s2)
        self.assert_num_klass_nodes(ps2, For, 1)
        # here there are 3 refs to 'i' since the declaration doesn't count as
        # a ref in the visitor
        #
        self.assert_num_ID_refs(ps2, 'i', 3)

        s3 = r'''
        void x(void)
        {
            for (int i = 0; i < 5; ++i) {
                x = 50;
            }
        }
        '''
        ps3 = self.parse(s3)
        self.assert_num_klass_nodes(ps3, For, 1)
        # here there are 2 refs to 'i' since the declaration doesn't count as
        # a ref in the visitor
        #
        self.assert_num_ID_refs(ps3, 'i', 2)

        s4 = r'''
        void x(void) {
            for (int i = 0;;)
                i;
        }
        '''
        ps4 = self.parse(s4)
        self.assert_num_ID_refs(ps4, 'i', 1)

    def _open_c_file(self, name):
        """ Find a c file by name, taking into account the current dir can be
            in a couple of typical places
        """
        testdir = os.path.dirname(__file__)
        name = os.path.join(testdir, 'c_files', name)
        assert os.path.exists(name)
        return io.open(name)

    def test_whole_file(self):
        # See how pycparser handles a whole, real C file.
        #
        with self._open_c_file('memmgr_with_h.c') as f:
            code = f.read()
        p = self.parse(code)

        self.assert_num_klass_nodes(p, FuncDef, 5)

        # each FuncDef also has a FuncDecl. 4 declarations
        # + 5 definitions, overall 9
        self.assert_num_klass_nodes(p, FuncDecl, 9)

        self.assert_num_klass_nodes(p, Typedef, 4)

        self.assertEqual(p.ext[4].coord.line, 88)
        self.assertEqual(p.ext[4].coord.file, "./memmgr.h")

        self.assertEqual(p.ext[6].coord.line, 10)
        self.assertEqual(p.ext[6].coord.file, "memmgr.c")

    def test_whole_file_with_stdio(self):
        # Parse a whole file with stdio.h included by cpp
        #
        with self._open_c_file('cppd_with_stdio_h.c') as f:
            code = f.read()
        p = self.parse(code)

        self.assertIsInstance(p.ext[0], Typedef)
        self.assertEqual(p.ext[0].coord.line, 213)
        self.assertEqual(p.ext[0].coord.file, r"D:\eli\cpp_stuff\libc_include/stddef.h")

        self.assertIsInstance(p.ext[-1], FuncDef)
        self.assertEqual(p.ext[-1].coord.line, 15)
        self.assertEqual(p.ext[-1].coord.file, "example_c_file.c")

        self.assertIsInstance(p.ext[-8], Typedef)
        self.assertIsInstance(p.ext[-8].type, TypeDecl)
        self.assertEqual(p.ext[-8].name, 'cookie_io_functions_t')


class TestCParser_typenames(TestCParser_base):
    """ Test issues related to the typedef-name problem.
    """
    def test_innerscope_typedef(self):
        # should fail since TT is not a type in bar
        s1 = r'''
            void foo() {
              typedef char TT;
              TT x;
            }
            void bar() {
              TT y;
            }
            '''
        self.assertRaises(ParseError, self.parse, s1)

        # should succeed since TT is not a type in bar
        s2 = r'''
            void foo() {
              typedef char TT;
              TT x;
            }
            void bar() {
                unsigned TT;
            }
            '''
        self.assertIsInstance(self.parse(s2), FileAST)

    def test_ambiguous_parameters(self):
        # From ISO/IEC 9899:TC2, 6.7.5.3.11:
        # "If, in a parameter declaration, an identifier can be treated either
        #  as a typedef name or as a parameter name, it shall be taken as a
        #  typedef name."

        # foo takes an int named aa
        # bar takes a function taking a TT
        s1 = r'''
        typedef char TT;
        int foo(int (aa));
        int bar(int (TT));
        '''
        s1_ast = self.parse(s1)
        self.assertEqual(expand_decl(s1_ast.ext[1].type.args.params[0]),
            ['Decl', 'aa', ['TypeDecl', ['IdentifierType', ['int']]]])
        self.assertEqual(expand_decl(s1_ast.ext[2].type.args.params[0]),
            ['Typename', ['FuncDecl',
                [['Typename', ['TypeDecl', ['IdentifierType', ['TT']]]]],
                ['TypeDecl', ['IdentifierType', ['int']]]]])

        # foo takes a function taking a char
        # bar takes a function taking a function taking a char
        s2 = r'''
        typedef char TT;
        int foo(int (aa (char)));
        int bar(int (TT (char)));
        '''
        s2_ast = self.parse(s2)
        self.assertEqual(expand_decl(s2_ast.ext[1].type.args.params[0]),
            ['Decl', 'aa', ['FuncDecl',
                [['Typename', ['TypeDecl', ['IdentifierType', ['char']]]]],
                ['TypeDecl', ['IdentifierType', ['int']]]]])
        self.assertEqual(expand_decl(s2_ast.ext[2].type.args.params[0]),
            ['Typename', ['FuncDecl',
                [['Typename', ['FuncDecl',
                    [['Typename', ['TypeDecl', ['IdentifierType', ['char']]]]],
                    ['TypeDecl', ['IdentifierType', ['TT']]]]]],
                ['TypeDecl', ['IdentifierType', ['int']]]]])


        # foo takes an int array named aa
        # bar takes a function taking a TT array
        s3 = r'''
        typedef char TT;
        int foo(int (aa[]));
        int bar(int (TT[]));
        '''
        s3_ast = self.parse(s3)
        self.assertEqual(expand_decl(s3_ast.ext[1].type.args.params[0]),
            ['Decl', 'aa', ['ArrayDecl', '', [], ['TypeDecl', ['IdentifierType', ['int']]]]])
        self.assertEqual(expand_decl(s3_ast.ext[2].type.args.params[0]),
            ['Typename', ['FuncDecl',
                [['Typename', ['ArrayDecl', '', [], ['TypeDecl', ['IdentifierType', ['TT']]]]]],
                ['TypeDecl', ['IdentifierType', ['int']]]]])

    def test_innerscope_reuse_typedef_name(self):
        # identifiers can be reused in inner scopes; the original should be
        # restored at the end of the block
        s1 = r'''
            typedef char TT;
            void foo(void) {
              unsigned TT;
              TT = 10;
            }
            TT x = 5;
            '''
        s1_ast = self.parse(s1)
        self.assertEqual(expand_decl(s1_ast.ext[1].body.block_items[0]),
            ['Decl', 'TT', ['TypeDecl', ['IdentifierType', ['unsigned']]]])
        self.assertEqual(expand_decl(s1_ast.ext[2]),
            ['Decl', 'x', ['TypeDecl', ['IdentifierType', ['TT']]]])

        # this should be recognized even with an initializer
        s2 = r'''
            typedef char TT;
            void foo(void) {
              unsigned TT = 10;
            }
            '''
        s2_ast = self.parse(s2)
        self.assertEqual(expand_decl(s2_ast.ext[1].body.block_items[0]),
            ['Decl', 'TT', ['TypeDecl', ['IdentifierType', ['unsigned']]]])

        # before the second local variable, TT is a type; after, it's a
        # variable
        s3 = r'''
            typedef char TT;
            void foo(void) {
              TT tt = sizeof(TT);
              unsigned TT = 10;
            }
            '''
        s3_ast = self.parse(s3)
        self.assertEqual(expand_decl(s3_ast.ext[1].body.block_items[0]),
            ['Decl', 'tt', ['TypeDecl', ['IdentifierType', ['TT']]]])
        self.assertEqual(expand_decl(s3_ast.ext[1].body.block_items[1]),
            ['Decl', 'TT', ['TypeDecl', ['IdentifierType', ['unsigned']]]])

        # a variable and its type can even share the same name
        s4 = r'''
            typedef char TT;
            void foo(void) {
              TT TT = sizeof(TT);
              unsigned uu = TT * 2;
            }
            '''
        s4_ast = self.parse(s4)
        self.assertEqual(expand_decl(s4_ast.ext[1].body.block_items[0]),
            ['Decl', 'TT', ['TypeDecl', ['IdentifierType', ['TT']]]])
        self.assertEqual(expand_decl(s4_ast.ext[1].body.block_items[1]),
            ['Decl', 'uu', ['TypeDecl', ['IdentifierType', ['unsigned']]]])

        # ensure an error is raised if a type, redeclared as a variable, is
        # used as a type
        s5 = r'''
            typedef char TT;
            void foo(void) {
              unsigned TT = 10;
              TT erroneous = 20;
            }
            '''
        self.assertRaises(ParseError, self.parse, s5)

        # reusing a type name should work with multiple declarators
        s6 = r'''
            typedef char TT;
            void foo(void) {
              unsigned TT, uu;
            }
            '''
        s6_ast = self.parse(s6)
        items = s6_ast.ext[1].body.block_items
        self.assertEqual(expand_decl(items[0]),
            ['Decl', 'TT', ['TypeDecl', ['IdentifierType', ['unsigned']]]])
        self.assertEqual(expand_decl(items[1]),
            ['Decl', 'uu', ['TypeDecl', ['IdentifierType', ['unsigned']]]])

        # reusing a type name should work after a pointer
        s7 = r'''
            typedef char TT;
            void foo(void) {
              unsigned * TT;
            }
            '''
        s7_ast = self.parse(s7)
        items = s7_ast.ext[1].body.block_items
        self.assertEqual(expand_decl(items[0]),
            ['Decl', 'TT', ['PtrDecl', ['TypeDecl', ['IdentifierType', ['unsigned']]]]])

        # redefine a name in the middle of a multi-declarator declaration
        s8 = r'''
            typedef char TT;
            void foo(void) {
                int tt = sizeof(TT), TT, uu = sizeof(TT);
                int uu = sizeof(tt);
            }
            '''
        s8_ast = self.parse(s8)
        items = s8_ast.ext[1].body.block_items
        self.assertEqual(expand_decl(items[0]),
            ['Decl', 'tt', ['TypeDecl', ['IdentifierType', ['int']]]])
        self.assertEqual(expand_decl(items[1]),
            ['Decl', 'TT', ['TypeDecl', ['IdentifierType', ['int']]]])
        self.assertEqual(expand_decl(items[2]),
            ['Decl', 'uu', ['TypeDecl', ['IdentifierType', ['int']]]])

        # Don't test this until we have support for it
        # self.assertEqual(expand_init(items[0].init),
        #     ['UnaryOp', 'sizeof', ['Typename', ['TypeDecl', ['IdentifierType', ['TT']]]]])
        # self.assertEqual(expand_init(items[2].init),
        #     ['UnaryOp', 'sizeof', ['ID', 'TT']])

    def test_parameter_reuse_typedef_name(self):
        # identifiers can be reused as parameter names; parameter name scope
        # begins and ends with the function body; it's important that TT is
        # used immediately before the LBRACE or after the RBRACE, to test
        # a corner case
        s1 = r'''
            typedef char TT;
            void foo(unsigned TT, TT bar) {
              TT = 10;
            }
            TT x = 5;
            '''
        s1_ast = self.parse(s1)
        self.assertEqual(expand_decl(s1_ast.ext[1].decl),
            ['Decl', 'foo',
                ['FuncDecl',
                    [   ['Decl', 'TT', ['TypeDecl', ['IdentifierType', ['unsigned']]]],
                        ['Decl', 'bar', ['TypeDecl', ['IdentifierType', ['TT']]]]],
                    ['TypeDecl', ['IdentifierType', ['void']]]]])

        # the scope of a parameter name in a function declaration ends at the
        # end of the declaration...so it is effectively never used; it's
        # important that TT is used immediately after the declaration, to
        # test a corner case
        s2 = r'''
            typedef char TT;
            void foo(unsigned TT, TT bar);
            TT x = 5;
            '''
        s2_ast = self.parse(s2)
        self.assertEqual(expand_decl(s2_ast.ext[1]),
            ['Decl', 'foo',
                ['FuncDecl',
                    [   ['Decl', 'TT', ['TypeDecl', ['IdentifierType', ['unsigned']]]],
                        ['Decl', 'bar', ['TypeDecl', ['IdentifierType', ['TT']]]]],
                    ['TypeDecl', ['IdentifierType', ['void']]]]])

        # ensure an error is raised if a type, redeclared as a parameter, is
        # used as a type
        s3 = r'''
            typedef char TT;
            void foo(unsigned TT, TT bar) {
              TT erroneous = 20;
            }
            '''
        self.assertRaises(ParseError, self.parse, s3)

    def test_nested_function_decls(self):
        # parameter names of nested function declarations must not escape into
        # the top-level function _definition's_ scope; the following must
        # succeed because TT is still a typedef inside foo's body
        s1 = r'''
            typedef char TT;
            void foo(unsigned bar(int TT)) {
              TT x = 10;
            }
            '''
        self.assertIsInstance(self.parse(s1), FileAST)

    def test_samescope_reuse_name(self):
        # a typedef name cannot be reused as an object name in the same scope
        s1 = r'''
            typedef char TT;
            char TT = 5;
            '''
        self.assertRaises(ParseError, self.parse, s1)

        # ...and vice-versa
        s2 = r'''
            char TT = 5;
            typedef char TT;
            '''
        self.assertRaises(ParseError, self.parse, s2)


if __name__ == '__main__':
    #~ suite = unittest.TestLoader().loadTestsFromNames(
        #~ ['test_c_parser.TestCParser_fundamentals.test_typedef'])

    #~ unittest.TextTestRunner(verbosity=2).run(suite)
    unittest.main()

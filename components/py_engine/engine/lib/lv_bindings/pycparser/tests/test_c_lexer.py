import re
import sys
import unittest

sys.path.insert(0, '..')
from pycparser.c_lexer import CLexer


def token_list(clex):
    return list(iter(clex.token, None))


def token_types(clex):
    return [i.type for i in token_list(clex)]


class TestCLexerNoErrors(unittest.TestCase):
    """ Test lexing of strings that are not supposed to cause
        errors. Therefore, the error_func passed to the lexer
        raises an exception.
    """
    def error_func(self, msg, line, column):
        self.fail(msg)

    def on_lbrace_func(self):
        pass

    def on_rbrace_func(self):
        pass

    def type_lookup_func(self, typ):
        if typ.startswith('mytype'):
            return True
        else:
            return False

    def setUp(self):
        self.clex = CLexer(self.error_func, lambda: None, lambda: None,
                           self.type_lookup_func)
        self.clex.build(optimize=False)

    def assertTokensTypes(self, str, types):
        self.clex.input(str)
        self.assertEqual(token_types(self.clex), types)

    def test_trivial_tokens(self):
        self.assertTokensTypes('1', ['INT_CONST_DEC'])
        self.assertTokensTypes('-', ['MINUS'])
        self.assertTokensTypes('volatile', ['VOLATILE'])
        self.assertTokensTypes('...', ['ELLIPSIS'])
        self.assertTokensTypes('++', ['PLUSPLUS'])
        self.assertTokensTypes('case int', ['CASE', 'INT'])
        self.assertTokensTypes('caseint', ['ID'])
        self.assertTokensTypes('$dollar cent$', ['ID', 'ID'])
        self.assertTokensTypes('i ^= 1;', ['ID', 'XOREQUAL', 'INT_CONST_DEC', 'SEMI'])

    def test_id_typeid(self):
        self.assertTokensTypes('myt', ['ID'])
        self.assertTokensTypes('mytype', ['TYPEID'])
        self.assertTokensTypes('mytype6 var', ['TYPEID', 'ID'])

    def test_integer_constants(self):
        self.assertTokensTypes('12', ['INT_CONST_DEC'])
        self.assertTokensTypes('12u', ['INT_CONST_DEC'])
        self.assertTokensTypes('12l', ['INT_CONST_DEC'])
        self.assertTokensTypes('199872Ul', ['INT_CONST_DEC'])
        self.assertTokensTypes('199872lU', ['INT_CONST_DEC'])
        self.assertTokensTypes('199872LL', ['INT_CONST_DEC'])
        self.assertTokensTypes('199872ull', ['INT_CONST_DEC'])
        self.assertTokensTypes('199872llu', ['INT_CONST_DEC'])
        self.assertTokensTypes('1009843200000uLL', ['INT_CONST_DEC'])
        self.assertTokensTypes('1009843200000LLu', ['INT_CONST_DEC'])

        self.assertTokensTypes('077', ['INT_CONST_OCT'])
        self.assertTokensTypes('0123456L', ['INT_CONST_OCT'])

        self.assertTokensTypes('0xf7', ['INT_CONST_HEX'])
        self.assertTokensTypes('0b110', ['INT_CONST_BIN'])
        self.assertTokensTypes('0x01202AAbbf7Ul', ['INT_CONST_HEX'])
        self.assertTokensTypes("'12'", ['INT_CONST_CHAR'])
        self.assertTokensTypes("'123'", ['INT_CONST_CHAR'])
        self.assertTokensTypes("'1AB4'", ['INT_CONST_CHAR'])
        self.assertTokensTypes(r"'1A\n4'", ['INT_CONST_CHAR'])

        # no 0 before x, so ID catches it
        self.assertTokensTypes('xf7', ['ID'])

        # - is MINUS, the rest a constnant
        self.assertTokensTypes('-1', ['MINUS', 'INT_CONST_DEC'])

    def test_special_names(self):
        self.assertTokensTypes('sizeof offsetof', ['SIZEOF', 'OFFSETOF'])

    def test_floating_constants(self):
        self.assertTokensTypes('1.5f', ['FLOAT_CONST'])
        self.assertTokensTypes('01.5', ['FLOAT_CONST'])
        self.assertTokensTypes('.15L', ['FLOAT_CONST'])
        self.assertTokensTypes('0.', ['FLOAT_CONST'])

        # but just a period is a period
        self.assertTokensTypes('.', ['PERIOD'])

        self.assertTokensTypes('3.3e-3', ['FLOAT_CONST'])
        self.assertTokensTypes('.7e25L', ['FLOAT_CONST'])
        self.assertTokensTypes('6.e+125f', ['FLOAT_CONST'])
        self.assertTokensTypes('666e666', ['FLOAT_CONST'])
        self.assertTokensTypes('00666e+3', ['FLOAT_CONST'])

        # but this is a hex integer + 3
        self.assertTokensTypes('0x0666e+3', ['INT_CONST_HEX', 'PLUS', 'INT_CONST_DEC'])

    def test_hexadecimal_floating_constants(self):
        self.assertTokensTypes('0xDE.488641p0', ['HEX_FLOAT_CONST'])
        self.assertTokensTypes('0x.488641p0', ['HEX_FLOAT_CONST'])
        self.assertTokensTypes('0X12.P0', ['HEX_FLOAT_CONST'])

    def test_char_constants(self):
        self.assertTokensTypes(r"""'x'""", ['CHAR_CONST'])
        self.assertTokensTypes(r"""L'x'""", ['WCHAR_CONST'])
        self.assertTokensTypes(r"""'\t'""", ['CHAR_CONST'])
        self.assertTokensTypes(r"""'\''""", ['CHAR_CONST'])
        self.assertTokensTypes(r"""'\?'""", ['CHAR_CONST'])
        self.assertTokensTypes(r"""'\0'""", ['CHAR_CONST'])
        self.assertTokensTypes(r"""'\012'""", ['CHAR_CONST'])
        self.assertTokensTypes(r"""'\x2f'""", ['CHAR_CONST'])
        self.assertTokensTypes(r"""'\x2f12'""", ['CHAR_CONST'])
        self.assertTokensTypes(r"""L'\xaf'""", ['WCHAR_CONST'])

    def test_on_rbrace_lbrace(self):
        braces = []
        def on_lbrace():
            braces.append('{')
        def on_rbrace():
            braces.append('}')
        clex = CLexer(self.error_func, on_lbrace, on_rbrace,
                      self.type_lookup_func)
        clex.build(optimize=False)
        clex.input('hello { there } } and again }}{')
        token_list(clex)
        self.assertEqual(braces, ['{', '}', '}', '}', '}', '{'])

    def test_string_literal(self):
        self.assertTokensTypes('"a string"', ['STRING_LITERAL'])
        self.assertTokensTypes('L"ing"', ['WSTRING_LITERAL'])
        self.assertTokensTypes(
            '"i am a string too \t"',
            ['STRING_LITERAL'])
        self.assertTokensTypes(
            r'''"esc\ape \"\'\? \0234 chars \rule"''',
            ['STRING_LITERAL'])
        self.assertTokensTypes(
            r'''"hello 'joe' wanna give it a \"go\"?"''',
            ['STRING_LITERAL'])
        self.assertTokensTypes(
            '"\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123"',
            ['STRING_LITERAL'])
        # Note: a-zA-Z and '.-~^_!=&;,' are allowed as escape chars to support #line
        # directives with Windows paths as filenames (..\..\dir\file)
        self.assertTokensTypes(
            r'"\x"',
            ['STRING_LITERAL'])
        self.assertTokensTypes(
            r'"\a\b\c\d\e\f\g\h\i\j\k\l\m\n\o\p\q\r\s\t\u\v\w\x\y\z\A\B\C\D\E\F\G\H\I\J\K\L\M\N\O\P\Q\R\S\T\U\V\W\X\Y\Z"',
            ['STRING_LITERAL'])
        self.assertTokensTypes(
            r'"C:\x\fa\x1e\xited"',
            ['STRING_LITERAL'])
        # The lexer is permissive and allows decimal escapes (not just octal)
        self.assertTokensTypes(
            r'"jx\9"',
            ['STRING_LITERAL'])
        self.assertTokensTypes(
            r'"fo\9999999"',
            ['STRING_LITERAL'])

    def test_mess(self):
        self.assertTokensTypes(
            r'[{}]()',
            ['LBRACKET',
                'LBRACE', 'RBRACE',
            'RBRACKET',
            'LPAREN', 'RPAREN'])

        self.assertTokensTypes(
            r'()||!C&~Z?J',
            ['LPAREN', 'RPAREN',
            'LOR',
            'LNOT', 'ID',
            'AND',
            'NOT', 'ID',
            'CONDOP', 'ID'])

        self.assertTokensTypes(
            r'+-*/%|||&&&^><>=<===!=',
            ['PLUS', 'MINUS', 'TIMES', 'DIVIDE', 'MOD',
            'LOR', 'OR',
            'LAND', 'AND',
            'XOR',
            'GT', 'LT', 'GE', 'LE', 'EQ', 'NE'])

        self.assertTokensTypes(
            r'++--->?.,;:',
            ['PLUSPLUS', 'MINUSMINUS',
            'ARROW', 'CONDOP',
            'PERIOD', 'COMMA', 'SEMI', 'COLON'])

    def test_exprs(self):
        self.assertTokensTypes(
            'bb-cc',
            ['ID', 'MINUS', 'ID'])

        self.assertTokensTypes(
            'foo & 0xFF',
            ['ID', 'AND', 'INT_CONST_HEX'])

        self.assertTokensTypes(
            '(2+k) * 62',
            ['LPAREN', 'INT_CONST_DEC', 'PLUS', 'ID',
            'RPAREN', 'TIMES', 'INT_CONST_DEC'],)

        self.assertTokensTypes(
            'x | y >> z',
            ['ID', 'OR', 'ID', 'RSHIFT', 'ID'])

        self.assertTokensTypes(
            'x <<= z << 5',
            ['ID', 'LSHIFTEQUAL', 'ID', 'LSHIFT', 'INT_CONST_DEC'])

        self.assertTokensTypes(
            'x = y > 0 ? y : -6',
            ['ID', 'EQUALS',
                'ID', 'GT', 'INT_CONST_OCT',
                'CONDOP',
                'ID',
                'COLON',
                'MINUS', 'INT_CONST_DEC'])

        self.assertTokensTypes(
            'a+++b',
            ['ID', 'PLUSPLUS', 'PLUS', 'ID'])

    def test_statements(self):
        self.assertTokensTypes(
            'for (int i = 0; i < n; ++i)',
            ['FOR', 'LPAREN',
                        'INT', 'ID', 'EQUALS', 'INT_CONST_OCT', 'SEMI',
                        'ID', 'LT', 'ID', 'SEMI',
                        'PLUSPLUS', 'ID',
                    'RPAREN'])

        self.assertTokensTypes(
            'self: goto self;',
            ['ID', 'COLON', 'GOTO', 'ID', 'SEMI'])

        self.assertTokensTypes(
            """ switch (typ)
                {
                    case TYPE_ID:
                        m = 5;
                        break;
                    default:
                        m = 8;
                }""",
            ['SWITCH', 'LPAREN', 'ID', 'RPAREN',
                'LBRACE',
                    'CASE', 'ID', 'COLON',
                        'ID', 'EQUALS', 'INT_CONST_DEC', 'SEMI',
                        'BREAK', 'SEMI',
                    'DEFAULT', 'COLON',
                        'ID', 'EQUALS', 'INT_CONST_DEC', 'SEMI',
                'RBRACE'])

    def test_preprocessor_line(self):
        self.assertTokensTypes('#abracadabra', ['PPHASH', 'ID'])

        str = r"""
        546
        #line 66 "kwas\df.h"
        id 4
        dsf
        # 9
        armo
        #line 10 "..\~..\test.h"
        tok1
        #line 99999 "include/me.h"
        tok2
        """

        #~ self.clex.filename
        self.clex.input(str)
        self.clex.reset_lineno()

        t1 = self.clex.token()
        self.assertEqual(t1.type, 'INT_CONST_DEC')
        self.assertEqual(t1.lineno, 2)

        t2 = self.clex.token()
        self.assertEqual(t2.type, 'ID')
        self.assertEqual(t2.value, 'id')
        self.assertEqual(t2.lineno, 66)
        self.assertEqual(self.clex.filename, r'kwas\df.h')

        for i in range(3):
            t = self.clex.token()

        self.assertEqual(t.type, 'ID')
        self.assertEqual(t.value, 'armo')
        self.assertEqual(t.lineno, 9)
        self.assertEqual(self.clex.filename, r'kwas\df.h')

        t4 = self.clex.token()
        self.assertEqual(t4.type, 'ID')
        self.assertEqual(t4.value, 'tok1')
        self.assertEqual(t4.lineno, 10)
        self.assertEqual(self.clex.filename, r'..\~..\test.h')

        t5 = self.clex.token()
        self.assertEqual(t5.type, 'ID')
        self.assertEqual(t5.value, 'tok2')
        self.assertEqual(t5.lineno, 99999)
        self.assertEqual(self.clex.filename, r'include/me.h')

    def test_preprocessor_line_funny(self):
        str = r'''
        #line 10 "..\6\joe.h"
        10
        '''
        self.clex.input(str)
        self.clex.reset_lineno()

        t1 = self.clex.token()
        self.assertEqual(t1.type, 'INT_CONST_DEC')
        self.assertEqual(t1.lineno, 10)
        self.assertEqual(self.clex.filename, r'..\6\joe.h')


    def test_preprocessor_pragma(self):
        str = '''
        42
        #pragma
        #pragma helo me
        #pragma once
        # pragma omp parallel private(th_id)
        #\tpragma {pack: 2, smack: 3}
        #pragma <includeme.h> "nowit.h"
        #pragma "string"
        #pragma somestring="some_other_string"
        #pragma id 124124 and numbers 0235495
        59
        '''
        # Check that pragmas are tokenized, including trailing string
        self.clex.input(str)
        self.clex.reset_lineno()

        t1 = self.clex.token()
        self.assertEqual(t1.type, 'INT_CONST_DEC')

        t2 = self.clex.token()
        self.assertEqual(t2.type, 'PPPRAGMA')

        t3 = self.clex.token()
        self.assertEqual(t3.type, 'PPPRAGMA')

        t4 = self.clex.token()
        self.assertEqual(t4.type, 'PPPRAGMASTR')
        self.assertEqual(t4.value, 'helo me')

        for i in range(3):
            t = self.clex.token()

        t5 = self.clex.token()
        self.assertEqual(t5.type, 'PPPRAGMASTR')
        self.assertEqual(t5.value, 'omp parallel private(th_id)')

        for i in range(5):
            ta = self.clex.token()
            self.assertEqual(ta.type, 'PPPRAGMA')
            tb = self.clex.token()
            self.assertEqual(tb.type, 'PPPRAGMASTR')

        t6 = self.clex.token()
        self.assertEqual(t6.type, 'INT_CONST_DEC')
        self.assertEqual(t6.lineno, 12)



# Keeps all the errors the lexer spits in one place, to allow
# easier modification if the error syntax changes.
#
ERR_ILLEGAL_CHAR    = 'Illegal character'
ERR_OCTAL           = 'Invalid octal constant'
ERR_UNMATCHED_QUOTE = 'Unmatched \''
ERR_INVALID_CCONST  = 'Invalid char constant'
ERR_STRING_ESCAPE   = 'String contains invalid escape'

ERR_FILENAME_BEFORE_LINE    = 'filename before line'
ERR_LINENUM_MISSING         = 'line number missing'
ERR_INVALID_LINE_DIRECTIVE  = 'invalid #line directive'


class TestCLexerErrors(unittest.TestCase):
    """ Test lexing of erroneous strings.
        Works by passing an error functions that saves the error
        in an attribute for later perusal.
    """
    def error_func(self, msg, line, column):
        self.error = msg

    def on_lbrace_func(self):
        pass

    def on_rbrace_func(self):
        pass

    def type_lookup_func(self, typ):
        return False

    def setUp(self):
        self.clex = CLexer(self.error_func, self.on_lbrace_func,
                self.on_rbrace_func, self.type_lookup_func)
        self.clex.build(optimize=False)
        self.error = ""

    def assertLexerError(self, str, error_like):
        # feed the string to the lexer
        self.clex.input(str)

        # Pulls all tokens from the string. Errors will
        # be written into self.error by the error_func
        # callback
        #
        token_types(self.clex)

        # compare the error to the expected
        self.assertTrue(re.search(error_like, self.error),
            "\nExpected error matching: %s\nGot: %s" %
                (error_like, self.error))

        # clear last error, for the sake of subsequent invocations
        self.error = ""

    def test_trivial_tokens(self):
        self.assertLexerError('@', ERR_ILLEGAL_CHAR)
        self.assertLexerError('`', ERR_ILLEGAL_CHAR)
        self.assertLexerError('\\', ERR_ILLEGAL_CHAR)

    def test_integer_constants(self):
        self.assertLexerError('029', ERR_OCTAL)
        self.assertLexerError('012345678', ERR_OCTAL)

    def test_char_constants(self):
        self.assertLexerError("'", ERR_UNMATCHED_QUOTE)
        self.assertLexerError("'b\n", ERR_UNMATCHED_QUOTE)
        self.assertLexerError("'\\xaa\n'", ERR_UNMATCHED_QUOTE)
        
        self.assertLexerError(r"'123\12a'", ERR_INVALID_CCONST)
        self.assertLexerError(r"'123\xabg'", ERR_INVALID_CCONST)
        self.assertLexerError("''", ERR_INVALID_CCONST)
        self.assertLexerError("'abcjx'", ERR_INVALID_CCONST)
        self.assertLexerError(r"'\*'", ERR_INVALID_CCONST)

    def test_string_literals(self):
        self.assertLexerError(r'"jx\`"', ERR_STRING_ESCAPE)
        self.assertLexerError(r'"hekllo\* on ix"', ERR_STRING_ESCAPE)
        self.assertLexerError(r'L"hekllo\* on ix"', ERR_STRING_ESCAPE)
        # Should not suffer from slow backtracking
        self.assertLexerError(r'"\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\`\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123"', ERR_STRING_ESCAPE)
        self.assertLexerError(r'"\xf1\x23\xf1\x23\xf1\x23\xf1\x23\xf1\x23\xf1\x23\xf1\x23\xf1\x23\xf1\x23\x23\`\xf1\x23\xf1\x23\xf1\x23\xf1\x23\xf1\x23\xf1\x23\xf1\x23\xf1\x23\xf1\x23\xf1\x23"', ERR_STRING_ESCAPE)
        # Should not suffer from slow backtracking when there's no end quote
        self.assertLexerError(r'"\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\`\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\123\12\123456', ERR_ILLEGAL_CHAR)
        self.assertLexerError(r'"\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\`\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x23\x2\x23456', ERR_ILLEGAL_CHAR)

    def test_preprocessor(self):
        self.assertLexerError('#line "ka"', ERR_FILENAME_BEFORE_LINE)
        self.assertLexerError('#line df', ERR_INVALID_LINE_DIRECTIVE)
        self.assertLexerError('#line \n', ERR_LINENUM_MISSING)


if __name__ == '__main__':
    unittest.main()

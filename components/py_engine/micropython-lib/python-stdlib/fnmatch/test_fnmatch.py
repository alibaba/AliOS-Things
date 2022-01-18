"""Test cases for the fnmatch module."""

from test import support
import unittest

from fnmatch import fnmatch, fnmatchcase, translate, filter


class FnmatchTestCase(unittest.TestCase):
    def check_match(self, filename, pattern, should_match=1, fn=fnmatch):
        if should_match:
            self.assertTrue(
                fn(filename, pattern), "expected %r to match pattern %r" % (filename, pattern)
            )
        else:
            self.assertTrue(
                not fn(filename, pattern),
                "expected %r not to match pattern %r" % (filename, pattern),
            )

    def test_fnmatch(self):
        check = self.check_match
        check("abc", "abc")
        check("abc", "?*?")
        check("abc", "???*")
        check("abc", "*???")
        check("abc", "???")
        check("abc", "*")
        check("abc", "ab[cd]")
        check("abc", "ab[!de]")
        check("abc", "ab[de]", 0)
        check("a", "??", 0)
        check("a", "b", 0)

        # these test that '\' is handled correctly in character sets;
        # see SF bug #409651
        check("\\", r"[\]")
        check("a", r"[!\]")
        check("\\", r"[!\]", 0)

        # test that filenames with newlines in them are handled correctly.
        # http://bugs.python.org/issue6665
        check("foo\nbar", "foo*")
        check("foo\nbar\n", "foo*")
        check("\nfoo", "foo*", False)
        check("\n", "*")

    def _test_mix_bytes_str(self):
        self.assertRaises(TypeError, fnmatch, "test", b"*")
        self.assertRaises(TypeError, fnmatch, b"test", "*")
        self.assertRaises(TypeError, fnmatchcase, "test", b"*")
        self.assertRaises(TypeError, fnmatchcase, b"test", "*")

    def test_fnmatchcase(self):
        check = self.check_match
        check("AbC", "abc", 0, fnmatchcase)
        check("abc", "AbC", 0, fnmatchcase)

    @unittest.skip("unsupported on MicroPython")
    def test_bytes(self):
        self.check_match(b"test", b"te*")
        self.check_match(b"test\xff", b"te*\xff")
        self.check_match(b"foo\nbar", b"foo*")


class TranslateTestCase(unittest.TestCase):
    def test_translate(self):
        self.assertEqual(translate("*"), "(?ms).*\Z")
        self.assertEqual(translate("?"), "(?ms).\Z")
        self.assertEqual(translate("a?b*"), "(?ms)a.b.*\Z")
        self.assertEqual(translate("[abc]"), "(?ms)[abc]\Z")
        self.assertEqual(translate("[]]"), "(?ms)[]]\Z")
        self.assertEqual(translate("[!x]"), "(?ms)[^x]\Z")
        self.assertEqual(translate("[^x]"), "(?ms)[\\^x]\Z")
        self.assertEqual(translate("[x"), "(?ms)\\[x\Z")


class FilterTestCase(unittest.TestCase):
    def test_filter(self):
        self.assertEqual(filter(["a", "b"], "a"), ["a"])


def test_main():
    support.run_unittest(FnmatchTestCase, TranslateTestCase, FilterTestCase)


if __name__ == "__main__":
    test_main()

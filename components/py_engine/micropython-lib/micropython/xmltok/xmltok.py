TEXT = "TEXT"
START_TAG = "START_TAG"
# START_TAG_DONE = "START_TAG_DONE"
END_TAG = "END_TAG"
PI = "PI"
# PI_DONE = "PI_DONE"
ATTR = "ATTR"
# ATTR_VAL = "ATTR_VAL"


class XMLSyntaxError(Exception):
    pass


class XMLTokenizer:
    def __init__(self, f):
        self.f = f
        self.nextch()

    def curch(self):
        return self.c

    def getch(self):
        c = self.c
        self.nextch()
        return c

    def eof(self):
        return self.c == ""

    def nextch(self):
        self.c = self.f.read(1)
        if not self.c:
            raise StopIteration
        return self.c

    def skip_ws(self):
        while self.curch().isspace():
            self.nextch()

    def isident(self):
        self.skip_ws()
        return self.curch().isalpha()

    def getident(self):
        self.skip_ws()
        ident = ""
        while True:
            c = self.curch()
            if not (c.isalpha() or c.isdigit() or c in "_-."):
                break
            ident += self.getch()
        return ident

    def getnsident(self):
        ns = ""
        ident = self.getident()
        if self.curch() == ":":
            self.nextch()
            ns = ident
            ident = self.getident()
        return (ns, ident)

    def match(self, c):
        self.skip_ws()
        if self.curch() == c:
            self.nextch()
            return True
        return False

    def expect(self, c):
        if not self.match(c):
            raise XMLSyntaxError

    def lex_attrs_till(self):
        while self.isident():
            attr = self.getnsident()
            # yield (ATTR, attr)
            self.expect("=")
            self.expect('"')
            val = ""
            while self.curch() != '"':
                val += self.getch()
            # yield (ATTR_VAL, val)
            self.expect('"')
            yield (ATTR, attr, val)

    def tokenize(self):
        while not self.eof():
            if self.match("<"):
                if self.match("/"):
                    yield (END_TAG, self.getnsident())
                    self.expect(">")
                elif self.match("?"):
                    yield (PI, self.getident())
                    yield from self.lex_attrs_till()
                    self.expect("?")
                    self.expect(">")
                elif self.match("!"):
                    self.expect("-")
                    self.expect("-")
                    last3 = ""
                    while True:
                        last3 = last3[-2:] + self.getch()
                        if last3 == "-->":
                            break
                else:
                    tag = self.getnsident()
                    yield (START_TAG, tag)
                    yield from self.lex_attrs_till()
                    if self.match("/"):
                        yield (END_TAG, tag)
                    self.expect(">")
            else:
                text = ""
                while self.curch() != "<":
                    text += self.getch()
                if text:
                    yield (TEXT, text)


def gfind(gen, pred):
    for i in gen:
        if pred(i):
            return i


def text_of(gen, tag):
    # Return text content of a leaf tag
    def match_tag(t):
        if t[0] != START_TAG:
            return False
        if isinstance(tag, ()):
            return t[1] == tag
        return t[1][1] == tag

    gfind(gen, match_tag)
    # Assumes no attributes
    t, val = next(gen)
    assert t == TEXT
    return val


def tokenize(file):
    return XMLTokenizer(file).tokenize()

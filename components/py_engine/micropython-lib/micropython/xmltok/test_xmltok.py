import xmltok

expected = [
    ("PI", "xml"),
    ("ATTR", ("", "version"), "1.0"),
    ("START_TAG", ("s", "Envelope")),
    ("ATTR", ("xmlns", "s"), "http://schemas.xmlsoap.org/soap/envelope/"),
    ("ATTR", ("s", "encodingStyle"), "http://schemas.xmlsoap.org/soap/encoding/"),
    ("START_TAG", ("s", "Body")),
    ("START_TAG", ("u", "GetConnectionTypeInfo")),
    ("ATTR", ("xmlns", "u"), "urn:schemas-upnp-org:service:WANIPConnection:1"),
    ("TEXT", "foo bar\n  baz\n  \n"),
    ("END_TAG", ("u", "GetConnectionTypeInfo")),
    ("END_TAG", ("s", "Body")),
    ("END_TAG", ("s", "Envelope")),
]

dir = "."
if "/" in __file__:
    dir = __file__.rsplit("/", 1)[0]

ex = iter(expected)
for i in xmltok.tokenize(open(dir + "/test.xml")):
    # print(i)
    assert i == next(ex)

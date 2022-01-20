import string

assert string.translate("foobar", {ord("o"): "foo", ord("b"): 32, ord("r"): None}) == "ffoofoo a"

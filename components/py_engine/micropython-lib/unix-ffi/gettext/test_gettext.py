import gettext

msg = gettext.gettext("yes")
assert msg == "yes"

msg = gettext.ngettext("one", "two", 1)
assert msg == "one"

msg = gettext.ngettext("one", "two", 2)
assert msg == "two"

msg = gettext.ngettext("one", "two", 0)
assert msg == "two"

msg = gettext.ngettext("one", "two", "three")
assert msg == "two"

# Pretty-printer utilities.
# Copyright (C) 2013-2014 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import gdb.printing

class BoundPrinter:
    """Adds size field to a _rawbound128 type."""

    def __init__ (self, val):
        self.val = val

    def to_string (self):
        upper = self.val["ubound"]
        lower = self.val["lbound"]
        size  = (long) ((upper) - (lower))
        if size > -1:
            size = size + 1
        result = '{lbound = %s, ubound = %s} : size %s' % (lower, upper, size)
        return result

# There are two pattern matching used: first one is related to a library
# second is related to the type. Since we are displaying a register all
# libraries are accepted. Type to be processed is the same present
# in the xml file.

def build_pretty_printer ():
    pp = gdb.printing.RegexpCollectionPrettyPrinter (".*")
    pp.add_printer ('bound', '^__gdb_builtin_type_bound128', BoundPrinter)
    return pp

gdb.printing.register_pretty_printer (gdb.current_objfile (),
                                      build_pretty_printer ())

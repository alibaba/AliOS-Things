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

import gdb

# This small code snippet deals with problem of strings in Python 2.x
# and Python 3.x.  Python 2.x has str and unicode classes which are
# sub-classes of basestring.  In Python 3.x all strings are encoded
# and basestring has been removed.
try:
    basestring
except NameError:
    basestring = str

class FrameDecorator(object):
    """Basic implementation of a Frame Decorator"""

    """ This base frame decorator decorates a frame or another frame
    decorator, and provides convenience methods.  If this object is
    wrapping a frame decorator, defer to that wrapped object's method
    if it has one.  This allows for frame decorators that have
    sub-classed FrameDecorator object, but also wrap other frame
    decorators on the same frame to correctly execute.

    E.g

    If the result of frame filters running means we have one gdb.Frame
    wrapped by multiple frame decorators, all sub-classed from
    FrameDecorator, the resulting hierarchy will be:

    Decorator1
      -- (wraps) Decorator2
        -- (wraps) FrameDecorator
          -- (wraps) gdb.Frame

    In this case we have two frame decorators, both of which are
    sub-classed from FrameDecorator.  If Decorator1 just overrides the
    'function' method, then all of the other methods are carried out
    by the super-class FrameDecorator.  But Decorator2 may have
    overriden other methods, so FrameDecorator will look at the
    'base' parameter and defer to that class's methods.  And so on,
    down the chain."""

    # 'base' can refer to a gdb.Frame or another frame decorator.  In
    # the latter case, the child class will have called the super
    # method and _base will be an object conforming to the Frame Filter
    # class.
    def __init__(self, base):
        self._base = base

    @staticmethod
    def _is_limited_frame(frame):
        """Internal utility to determine if the frame is special or
        limited."""
        sal = frame.find_sal()

        if (not sal.symtab or not sal.symtab.filename
            or frame.type() == gdb.DUMMY_FRAME
            or frame.type() == gdb.SIGTRAMP_FRAME):

            return True

        return False

    def elided(self):
        """Return any elided frames that this class might be
        wrapping, or None."""
        if hasattr(self._base, "elided"):
            return self._base.elided()

        return None

    def function(self):
        """ Return the name of the frame's function or an address of
        the function of the frame.  First determine if this is a
        special frame.  If not, try to determine filename from GDB's
        frame internal function API.  Finally, if a name cannot be
        determined return the address.  If this function returns an
        address, GDB will attempt to determine the function name from
        its internal minimal symbols store (for example, for inferiors
        without debug-info)."""

        # Both gdb.Frame, and FrameDecorator have a method called
        # "function", so determine which object this is.
        if not isinstance(self._base, gdb.Frame):
            if hasattr(self._base, "function"):
                # If it is not a gdb.Frame, and there is already a
                # "function" method, use that.
                return self._base.function()

        frame = self.inferior_frame()

        if frame.type() == gdb.DUMMY_FRAME:
            return "<function called from gdb>"
        elif frame.type() == gdb.SIGTRAMP_FRAME:
            return "<signal handler called>"

        func = frame.function()

        # If we cannot determine the function name, return the
        # address.  If GDB detects an integer value from this function
        # it will attempt to find the function name from minimal
        # symbols via its own internal functions.
        if func == None:
            pc = frame.pc()
            return pc

        return str(func)

    def address(self):
        """ Return the address of the frame's pc"""

        if hasattr(self._base, "address"):
            return self._base.address()

        frame = self.inferior_frame()
        return frame.pc()

    def filename(self):
        """ Return the filename associated with this frame, detecting
        and returning the appropriate library name is this is a shared
        library."""

        if hasattr(self._base, "filename"):
            return self._base.filename()

        frame = self.inferior_frame()
        sal = frame.find_sal()
        if not sal.symtab or not sal.symtab.filename:
            pc = frame.pc()
            return gdb.solib_name(pc)
        else:
            return sal.symtab.filename

    def frame_args(self):
        """ Return an iterable of frame arguments for this frame, if
        any.  The iterable object contains objects conforming with the
        Symbol/Value interface.  If there are no frame arguments, or
        if this frame is deemed to be a special case, return None."""

        if hasattr(self._base, "frame_args"):
            return self._base.frame_args()

        frame = self.inferior_frame()
        if self._is_limited_frame(frame):
            return None

        args = FrameVars(frame)
        return args.fetch_frame_args()

    def frame_locals(self):
        """ Return an iterable of local variables for this frame, if
        any.  The iterable object contains objects conforming with the
        Symbol/Value interface.  If there are no frame locals, or if
        this frame is deemed to be a special case, return None."""

        if hasattr(self._base, "frame_locals"):
            return self._base.frame_locals()

        frame = self.inferior_frame()
        if self._is_limited_frame(frame):
            return None

        args = FrameVars(frame)
        return args.fetch_frame_locals()

    def line(self):
        """ Return line number information associated with the frame's
        pc.  If symbol table/line information does not exist, or if
        this frame is deemed to be a special case, return None"""

        if hasattr(self._base, "line"):
            return self._base.line()

        frame = self.inferior_frame()
        if self._is_limited_frame(frame):
            return None

        sal = frame.find_sal()
        if (sal):
            return sal.line
        else:
            return None

    def inferior_frame(self):
        """ Return the gdb.Frame underpinning this frame decorator."""

        # If 'base' is a frame decorator, we want to call its inferior
        # frame method.  If '_base' is a gdb.Frame, just return that.
        if hasattr(self._base, "inferior_frame"):
            return self._base.inferior_frame()
        return self._base

class SymValueWrapper(object):
    """A container class conforming to the Symbol/Value interface
    which holds frame locals or frame arguments."""
    def __init__(self, symbol, value):
        self.sym = symbol
        self.val = value

    def value(self):
        """ Return the value associated with this symbol, or None"""
        return self.val

    def symbol(self):
        """ Return the symbol, or Python text, associated with this
        symbol, or None"""
        return self.sym

class FrameVars(object):

    """Utility class to fetch and store frame local variables, or
    frame arguments."""

    def __init__(self, frame):
        self.frame = frame
        self.symbol_class = {
            gdb.SYMBOL_LOC_STATIC: True,
            gdb.SYMBOL_LOC_REGISTER: True,
            gdb.SYMBOL_LOC_ARG: True,
            gdb.SYMBOL_LOC_REF_ARG: True,
            gdb.SYMBOL_LOC_LOCAL: True,
	    gdb.SYMBOL_LOC_REGPARM_ADDR: True,
	    gdb.SYMBOL_LOC_COMPUTED: True
            }

    def fetch_b(self, sym):
        """ Local utility method to determine if according to Symbol
        type whether it should be included in the iterator.  Not all
        symbols are fetched, and only symbols that return
        True from this method should be fetched."""

        # SYM may be a string instead of a symbol in the case of
        # synthetic local arguments or locals.  If that is the case,
        # always fetch.
        if isinstance(sym, basestring):
            return True

        sym_type = sym.addr_class

        return self.symbol_class.get(sym_type, False)

    def fetch_frame_locals(self):
        """Public utility method to fetch frame local variables for
        the stored frame.  Frame arguments are not fetched.  If there
        are no frame local variables, return an empty list."""
        lvars = []

        try:
            block = self.frame.block()
        except RuntimeError:
            block = None

        while block != None:
            if block.is_global or block.is_static:
                break
            for sym in block:
                if sym.is_argument:
                    continue;
                if self.fetch_b(sym):
                    lvars.append(SymValueWrapper(sym, None))

            block = block.superblock

        return lvars

    def fetch_frame_args(self):
        """Public utility method to fetch frame arguments for the
        stored frame.  Frame arguments are the only type fetched.  If
        there are no frame argument variables, return an empty list."""

        args = []

        try:
            block = self.frame.block()
        except RuntimeError:
            block = None

        while block != None:
            if block.function != None:
                break
            block = block.superblock

        if block != None:
            for sym in block:
                if not sym.is_argument:
                    continue;
                args.append(SymValueWrapper(sym, None))

        return args

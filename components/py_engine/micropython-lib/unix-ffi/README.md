Unix-specific libraries
=======================

These are libraries that will only run on the Unix port of MicroPython. There is some limited support for the Windows port too.

**Note:** This directory is largely unmaintained, although large breaking changes are not expected.

Background
----------

The libraries in this directory provide additional CPython compatibility using
the host operating system's native libraries.

This is implemented either by accessing the libraries directly via libffi, or by using built-in modules that are only available on the Unix port.

In theory, this allows you to use MicroPython as a more complete drop-in
replacement for CPython.

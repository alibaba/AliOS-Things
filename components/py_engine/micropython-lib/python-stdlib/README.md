CPython standard libraries
==========================

The libraries in this directory aim to provide compatible implementations of
standard libraries to allow existing Python code to run un-modified on
MicroPython.

Implementation
--------------

Many libraries are implemented in pure Python, often based on the original
CPython implementation. (e.g. `collections.defaultdict`)

Some libraries are based on or extend from the built-in "micro" modules in the
MicroPython firmware, providing additional functionality that didn't need to
be written in C (e.g. `collections`, `socket`, `struct`).


Future plans (ideas for contributors):
--------------------------------------

* Add README.md to each library explaining compatibility and limitations.

micropython-lib
===============

This is a repository of libraries designed to be useful for writing
MicroPython applications.

The libraries here fall into four categories corresponding to the four top-level directories:

 * **python-stdlib**: Compatible versions of modules from the [Python Standard Library](https://docs.python.org/3/library/). These should be drop-in replacements for the Python libraries, although many have reduced functionality or missing methods or classes (which may not be an issue for many most cases).

 * **python-ecosys**: Compatible, but reduced-functionality versions of modules from the larger Python ecosystem, for example that might be found in the [Python Package Index](https://pypi.org/).

* **micropython**: MicroPython-specific modules that do not have equivalents in other Python environments. These are typically hardware drivers or highly-optimised alternative implementations of functionality available in other Python modules.

 * **unix-ffi**: These modules are specifically for the MicroPython Unix port and provide access to operating-system and third-party libraries via FFI.

Usage
-----

Many libraries are self contained modules, and you can quickly get started by
copying the relevant Python file to your device. For example, to add the
`base64` library, you can directly copy `python-stdlib/base64/base64.py` to the `lib`
directory on your device.

Other libraries are packages, in which case you'll need to copy the directory instead. For example, to add `collections.defaultdict`, copy `collections/collections/__init__.py` and `collections.defaultdict/collections/defaultdict.py` to a directory named `lib/collections` on your device.

Future plans (and new contributor ideas)
----------------------------------------

* Provide compiled .mpy distributions.
* Develop a set of example programs using these libraries.
* Develop more MicroPython libraries for common tasks.
* Provide a replacement for the previous `upip` tool.

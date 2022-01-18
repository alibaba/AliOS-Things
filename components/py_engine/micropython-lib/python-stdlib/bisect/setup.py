import sys

# Remove current dir from sys.path, otherwise distutils will peek up our
# module instead of system.
sys.path.pop(0)
from setuptools import setup


def desc_dummy(name):
    return "Dummy %s module to MicroPython" % name


def desc_cpython(name):
    return "CPython %s module ported to MicroPython" % name


NAME = "bisect"

setup(
    name="micropython-" + NAME,
    version="0.5",
    description=desc_cpython(NAME),
    url="https://github.com/micropython/micropython/issues/405",
    author="CPython Developers",
    maintainer="MicroPython Developers",
    maintainer_email="micro-python@googlegroups.com",
    license="Python",
    py_modules=[NAME],
)

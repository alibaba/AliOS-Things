import sys

# Remove current dir from sys.path, otherwise setuptools will peek up our
# module instead of system's.
sys.path.pop(0)
from setuptools import setup

sys.path.append("..")
import sdist_upip

setup(
    name="micropython-hashlib",
    version="2.4.0-4",
    description="PyPy hashlib module ported to MicroPython",
    long_description="This is a module ported from PyPy standard library to be compatible with\nMicroPython interpreter. Usually, this means applying small patches for\nfeatures not supported (yet, or at all) in MicroPython. Sometimes, heavier\nchanges are required. Note that CPython modules are written with availability\nof vast resources in mind, and may not work for MicroPython ports with\nlimited heap. If you are affected by such a case, please help reimplement\nthe module from scratch.",
    url="https://github.com/micropython/micropython-lib",
    author="PyPy Developers",
    author_email="pypy-dev@python.org",
    maintainer="micropython-lib Developers",
    maintainer_email="micro-python@googlegroups.com",
    license="MIT",
    cmdclass={"sdist": sdist_upip.sdist},
    packages=["hashlib"],
)

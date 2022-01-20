import sys

# Remove current dir from sys.path, otherwise setuptools will peek up our
# module instead of system's.
sys.path.pop(0)
from setuptools import setup

sys.path.append("..")
import sdist_upip

setup(
    name="micropython-ucontextlib",
    version="0.1.1",
    description="ucontextlib module for MicroPython",
    long_description="Minimal subset of contextlib for MicroPython low-memory ports",
    url="https://github.com/micropython/micropython-lib",
    author="micropython-lib Developers",
    author_email="micro-python@googlegroups.com",
    maintainer="micropython-lib Developers",
    maintainer_email="micro-python@googlegroups.com",
    license="Python",
    cmdclass={"sdist": sdist_upip.sdist},
    py_modules=["ucontextlib"],
)

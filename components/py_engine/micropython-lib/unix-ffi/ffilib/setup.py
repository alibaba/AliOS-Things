import sys

# Remove current dir from sys.path, otherwise setuptools will peek up our
# module instead of system's.
sys.path.pop(0)
from setuptools import setup

sys.path.append("..")
import sdist_upip

setup(
    name="micropython-ffilib",
    version="0.1.3",
    description="MicroPython FFI helper module",
    long_description="MicroPython FFI helper module to easily interface with underlying shared libraries",
    url="https://github.com/micropython/micropython-lib",
    author="Damien George",
    author_email="micro-python@googlegroups.com",
    maintainer="micropython-lib Developers",
    maintainer_email="micro-python@googlegroups.com",
    license="MIT",
    cmdclass={"sdist": sdist_upip.sdist},
    py_modules=["ffilib"],
)

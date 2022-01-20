import sys

# Remove current dir from sys.path, otherwise setuptools will peek up our
# module instead of system's.
sys.path.pop(0)
from setuptools import setup

sys.path.append("..")
import sdist_upip

setup(
    name="micropython-upip",
    version="1.2.4",
    description="Simple package manager for MicroPython.",
    long_description="Simple self-hosted package manager for MicroPython (requires usocket, ussl, uzlib, uctypes builtin modules). Compatible only with packages without custom setup.py code.",
    url="https://github.com/micropython/micropython-lib",
    author="Paul Sokolovsky",
    author_email="micro-python@googlegroups.com",
    maintainer="micropython-lib Developers",
    maintainer_email="micro-python@googlegroups.com",
    license="MIT",
    cmdclass={"sdist": sdist_upip.sdist},
    py_modules=["upip", "upip_utarfile"],
)

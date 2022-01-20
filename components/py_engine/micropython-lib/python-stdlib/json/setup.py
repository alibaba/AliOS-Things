# import sys
# Remove current dir from sys.path, otherwise distutils will peek up our
# copy module instead of system.
# sys.path.pop(0)
from setuptools import setup

setup(
    name="micropython-json",
    version="0.1",
    description="CPython json package ported to MicroPython",
    url="https://github.com/micropython/micropython/issues/405",
    author="CPython Developers",
    maintainer="MicroPython Developers",
    maintainer_email="micro-python@googlegroups.com",
    license="Python",
    install_requires=["micropython-re-pcre"],
    packages=["json"],
)

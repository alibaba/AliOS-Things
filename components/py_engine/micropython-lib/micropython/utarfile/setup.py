import sys

# Remove current dir from sys.path, otherwise setuptools will peek up our
# module instead of system's.
sys.path.pop(0)
from setuptools import setup

sys.path.append("..")
import sdist_upip

setup(
    name="micropython-utarfile",
    version="0.3.2",
    description="utarfile module for MicroPython",
    long_description="Lightweight tarfile module subset",
    url="https://github.com/micropython/micropython-lib",
    author="Paul Sokolovsky",
    author_email="micro-python@googlegroups.com",
    maintainer="micropython-lib Developers",
    maintainer_email="micro-python@googlegroups.com",
    license="MIT",
    cmdclass={"sdist": sdist_upip.sdist},
    py_modules=["utarfile"],
)

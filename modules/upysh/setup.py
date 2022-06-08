import sys

# Remove current dir from sys.path, otherwise setuptools will peek up our
# module instead of system's.
sys.path.pop(0)
from setuptools import setup

sys.path.append("..")
import sdist_upip

setup(
    name="micropython-upysh",
    version="0.6.1",
    description="Minimalistic file shell using native Python syntax.",
    long_description="Minimalistic file shell using native Python syntax.",
    url="https://github.com/micropython/micropython-lib",
    author="micropython-lib Developers",
    author_email="micro-python@googlegroups.com",
    maintainer="micropython-lib Developers",
    maintainer_email="micro-python@googlegroups.com",
    license="MIT",
    cmdclass={"sdist": sdist_upip.sdist},
    py_modules=["upysh"],
)

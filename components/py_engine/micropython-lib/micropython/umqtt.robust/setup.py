import sys

# Remove current dir from sys.path, otherwise setuptools will peek up our
# module instead of system's.
sys.path.pop(0)
from setuptools import setup

sys.path.append("..")
import sdist_upip

setup(
    name="micropython-umqtt.robust",
    version="1.0.1",
    description='Lightweight MQTT client for MicroPython ("robust" version).',
    long_description=open("README.rst").read(),
    url="https://github.com/micropython/micropython-lib",
    author="Paul Sokolovsky",
    author_email="micro-python@googlegroups.com",
    maintainer="micropython-lib Developers",
    maintainer_email="micro-python@googlegroups.com",
    license="MIT",
    cmdclass={"sdist": sdist_upip.sdist},
    packages=["umqtt"],
)

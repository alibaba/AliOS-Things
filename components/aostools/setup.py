#!/usr/bin/env python
# -*- coding: utf-8 -*-

from distutils.command.build_ext import build_ext as _build_ext
from distutils.core import Extension
import os
import sys
import platform
import site
from codecs import open  # To use a consistent encoding

# Always prefer setuptools over distutils
from setuptools import setup, find_packages

def read(rel_path):
    here = os.path.abspath(os.path.dirname(__file__))
    with open(os.path.join(here, rel_path), 'r') as fp:
        return fp.read()

def get_version(rel_path):
    for line in read(rel_path).splitlines():
        if line.startswith('__version__'):
            # __version__ = "0.9"
            delim = '"' if '"' in line else "'"
            return line.split(delim)[1]
    else:
        raise RuntimeError("Unable to find version string.")

APP_NAME = 'aos-tools'

settings = dict()

thead_tools = []
if platform.system() == "Linux":
    thead_tools=[
        ('bin', ['aostools/build/product64']),
        ('bin', ['aostools/build/product32']),
        ('bin', ['aostools/build/gen_ldfile.sh']),
        ('/usr/local/bin', ['aostools/build/product64']),
        ('/usr/local/bin', ['aostools/build/product32']),
        ('/usr/local/bin', ['aostools/build/gen_ldfile.sh']),
        ('/usr/local/lib/aostools/script', [
            'aostools/script/aft_build.sh',
            'aostools/script/cdkgdbinit',
            'aostools/script/flash.init',
            'aostools/script/flash_prog.init',
            'aostools/script/pre_build.sh',
        ])]

requirements=[
        'scons>=3.0.0, <4.0.0',
        'requests_toolbelt',
        'threadpool',
        'smmap',
        'configparser==4.0.2',
        'pyyaml',
        'pyserial',
        'chardet']
if platform.system() == "Windows":
    requirements.append('pywin32')

settings.update(
    name=APP_NAME,
    version=get_version("aostools/cmd.py"),
    description='aos tools',
    author='Zhuzhg',
    author_email='zzg@ifnfn.com',
    packages=find_packages(),
    # packages = ['aostools', 'git', 'gitdb', 'yaml'],
    install_requires=requirements,

    license='BSD',
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Natural Language :: English',
        'License :: OSI Approved :: BSD License',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
    ],
    data_files=thead_tools,
    entry_points={
        'console_scripts': [
            'aos = aostools.cmd:main',
            'aos-tools = aostools.cmd:main',
            'cct = aostools.cmd:cct_main'
        ],
    },

    python_requires='>=2.7, !=3.0.*, !=3.1.*, !=3.2.*, !=3.3.*, !=3.4.*',
)


setup(**settings)

if platform.system() == "Linux":
    if os.getuid() != 0:
        install_path = os.path.join(site.USER_BASE, 'bin', 'product')
    else:
        install_path = os.path.join(sys.prefix, 'bin', 'product')

    architecture = platform.architecture()

    if architecture[0] == '64bit':
        product = install_path + '64'
    else:
        product = install_path + '32'

    print("create link from %s to %s" %(product, install_path))
    try:
        os.symlink(product, install_path)
    except:
        pass

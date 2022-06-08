import os, sys
try:
    from setuptools import setup
    from setuptools.command.install import install as _install
    from setuptools.command.sdist import sdist as _sdist
except ImportError:
    from distutils.core import setup
    from distutils.command.install import install as _install
    from distutils.command.sdist import sdist as _sdist


def _run_build_tables(dir):
    from subprocess import check_call
    # This is run inside the install staging directory (that had no .pyc files)
    # We don't want to generate any.
    # https://github.com/eliben/pycparser/pull/135
    check_call([sys.executable, '-B', '_build_tables.py'],
               cwd=os.path.join(dir, 'pycparser'))


class install(_install):
    def run(self):
        _install.run(self)
        self.execute(_run_build_tables, (self.install_lib,),
                     msg="Build the lexing/parsing tables")


class sdist(_sdist):
    def make_release_tree(self, basedir, files):
        _sdist.make_release_tree(self, basedir, files)
        self.execute(_run_build_tables, (basedir,),
                     msg="Build the lexing/parsing tables")


setup(
    # metadata
    name='pycparser',
    description='C parser in Python',
    long_description="""
        pycparser is a complete parser of the C language, written in
        pure Python using the PLY parsing library.
        It parses C code into an AST and can serve as a front-end for
        C compilers or analysis tools.
    """,
    license='BSD',
    version='2.20',
    author='Eli Bendersky',
    maintainer='Eli Bendersky',
    author_email='eliben@gmail.com',
    url='https://github.com/eliben/pycparser',
    platforms='Cross Platform',
    classifiers = [
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: BSD License',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
    ],
    python_requires=">=2.7, !=3.0.*, !=3.1.*, !=3.2.*, !=3.3.*",
    packages=['pycparser', 'pycparser.ply'],
    package_data={'pycparser': ['*.cfg']},
    cmdclass={'install': install, 'sdist': sdist},
)

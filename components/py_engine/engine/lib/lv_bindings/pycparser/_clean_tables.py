# Cleanup all tables and PYC files to ensure no PLY stuff is cached
from __future__ import print_function
import itertools
import fnmatch
import os, shutil

file_patterns = ('yacctab.*', 'lextab.*', '*.pyc', '__pycache__')


def do_cleanup(root):
    for path, dirs, files in os.walk(root):
        for file in itertools.chain(dirs, files):
            try:
                for pattern in file_patterns:
                    if fnmatch.fnmatch(file, pattern):
                        fullpath = os.path.join(path, file)
                        if os.path.isdir(fullpath):
                            shutil.rmtree(fullpath, ignore_errors=False)
                        else:
                            os.unlink(fullpath)
                        print('Deleted', fullpath)
            except OSError:
                pass


if __name__ == "__main__":
    do_cleanup('.')

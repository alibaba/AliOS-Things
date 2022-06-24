Basic benchmarking of parsing speed with pycparser.

The ``inputs`` directory contains preprocessed files taken from open source
projects.

``redis.c.pp`` taken from Redis. Generated with:

.. sourcecode::

   gcc -nostdinc -D'__attribute__(x)=' -E -Isrc/ -Ideps/hiredis -Ideps/linenoise -I$HOME/eli/pycparser/utils/fake_libc_include src/redis-cli.c

``tccgen.c.pp`` taken from TCC. Generated with:

.. sourcecode::

   gcc -nostdinc -D'__attribute__(x)=' -E -I.  -I$HOME/eli/pycparser/utils/fake_libc_include tccgen.c

``sqlite-btree.c.pp`` taken from SQLite. Generated with:

.. sourcecode::

   gcc -nostdinc -D'__attribute__(x)=' -E -I. -Isrc/ -I$HOME/eli/pycparser/utils/fake_libc_include src/btree.c

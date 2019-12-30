src     = Split('''
        src/adler32.c
        src/compress.c
        src/crc32.c
        src/deflate.c
        src/gzclose.c
        src/gzlib.c
        src/gzread.c
        src/gzwrite.c
        src/infback.c
        src/inffast.c
        src/inflate.c
        src/inftrees.c
        src/trees.c
        src/uncompr.c
        src/zutil.c
''')

component = aos_component('zlib', src)
component.add_global_includes('.')
component.add_includes('include')

src = Split('''
        CheckSumUtils.c
        crc.c
        digest_algorithm.c
        md5.c
''')

component = aos_component('digest_algorithm', src)

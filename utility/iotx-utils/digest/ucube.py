src = Split('''
    utils_base64.c
    utils_hmac.c
    utils_md5.c
    utils_sha1.c
''')

component = aos_component('libdigest', src)

component.add_includes('.', '../LITE-log', '../LITE-log', '../sdk-impl')

component.add_macros('DEBUG')




src = Split('''
    hashtable.c
''')

component = aos_component('hashtable', src)
component.add_includes('../../middleware/alink/protocol/os/')
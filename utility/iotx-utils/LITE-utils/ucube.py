src = Split('''
    json_parser.c
    json_token.c
    mem_stats.c
    string_utils.c
''')

component = aos_component('LITE-utils', src)

component.add_macros('DEBUG')
component.add_includes('.', '../LITE-log')

component.add_comp_deps('utility/iotx-utils/LITE-log')





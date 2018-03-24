src = Split('''
    json_parser.c
    json_token.c
    mem_stats.c
    string_utils.c
''')

component = aos_component('LITE-utils', src)

component.add_macro('DEBUG')
component.add_includes('.', '../LITE-log')

component.add_component_dependencis('utility/iotx-utils/LITE-log')





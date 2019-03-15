src = Split('''
    cJSON.c 
    cJSON_Utils.c
''')
component = aos_component('cjson', src)

component.add_global_includes('include')



src = Split('''
    ota_download.c
''')

component = aos_component('download_http', src)

component.add_includes('.','..','./socket')

component.add_component_dependencis('framework/fota/download/http/socket')


src = Split('''
        gateway_service.c
''')

component = aos_component('gateway', src)
component.add_global_macro('MESH_GATEWAY_SERVICE')
component.add_global_macro('GATEWAY_SDK')

component.add_component_dependencis('utility/cjson', 'kernel/protocols/mesh', 'framework/protocol/alink',
                                    'framework/gateway/devmgr', 'framework/gateway/msdp')

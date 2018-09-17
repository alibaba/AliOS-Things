src = Split('''
        gateway_service.c
''')

component = aos_component('gateway', src)
component.add_global_macros('MESH_GATEWAY_SERVICE')
component.add_global_macros('GATEWAY_SDK')

component.add_comp_deps('utility/cjson', 'network/umesh', 'middleware/alink/protocol',
                        'middleware/alink/gateway/devmgr', 'middleware/alink/gateway/msdp')

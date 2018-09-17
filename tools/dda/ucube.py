src = Split('''
   eloop.c 
   agent.c 
   msg.c 
   hal.c 
   config_parser.c 
   master.c
''')

component = aos_component('dda', src)

component.add_global_includes('include')

component.add_includes('network/umesh/include')
component.add_cflags('-Wall')
component.add_cflags('-Werror')

component.add_global_macros('CONFIG_AOS_DDA')
component.add_global_macros('CONFIG_AOS_DDM')

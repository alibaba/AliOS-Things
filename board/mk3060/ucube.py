src     = ['board.c']

component = aos_arch_component('board_mk3060', src)
component.add_component_dependencis('platform/mcu/moc108')
component.add_global_ld_file('memory.ld.S')


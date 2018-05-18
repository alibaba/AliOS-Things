src     = ['board.c']

component = aos_board_component('board_mk3060', 'moc108', src)
aos_global_config.add_ld_files('memory.ld.S')

supported_targets=="helloworld linuxapp meshapp uDataapp networkapp linkkitapp"
build_types="release"


src     = Split('''
        board.c
''')

component = aos_board_component('board_esp8266-demo', 'esp8266', src)

platform_options="wifi=1"
linux_only_targets="app_nomain"

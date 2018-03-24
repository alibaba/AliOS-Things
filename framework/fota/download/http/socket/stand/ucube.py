src = Split('''
    ota_socket.c
''')

component = aos_component('socket_stand', src)

component.add_includes('.','..')

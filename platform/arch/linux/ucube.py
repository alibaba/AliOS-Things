src = []

if aos_global_config.get('vcall') != 'posix':
    src.append('cpu_impl.c')

if aos_global_config.board == 'linuxhost':
    src.append('swap.S')

aos_component('linux', src)

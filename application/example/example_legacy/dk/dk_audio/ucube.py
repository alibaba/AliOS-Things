src = []

if aos_global_config.board == 'developerkit':
    src.append('developerkitaudio.c')

component = aos_component('developerkitaudio', src)
component.add_comp_deps('kernel/yloop', 'kernel/cli', 'board.developerkit.audio_hal')
component.add_global_macros('AOS_NO_WIFI')

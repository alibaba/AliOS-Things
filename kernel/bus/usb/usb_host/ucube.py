src = Split('''
    core/src/usb_host_core.c
    core/src/usb_host_device.c
    core/src/usb_host_os.c
    core/src/usb_host_pipe.c
    core/src/usb_host_request.c
    core/src/usb_host_transfer.c
    class/msd/src/usb_host_msd.c
''')

component = aos_component('usb_host', src)
component.add_cflags('-Wall')
component.add_cflags('-Werror')

component.add_global_macros('AOS_USB_HOST')
component.add_global_includes('.')
component.add_global_includes('core/include')
component.add_global_includes('class/msd/include')

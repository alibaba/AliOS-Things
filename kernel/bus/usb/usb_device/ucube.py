src = Split('''
    core/src/usb_device.c
    class/msd/src/usb_device_msd.c
    class/hid/src/usb_device_hid.c
    class/hid/src/usb_device_mouse.c
''')

component = aos_component('usb_device', src)
component.add_cflags('-Wall')
component.add_cflags('-Werror')

component.add_global_macros('AOS_USB_DEVICE')
component.add_global_includes('.')
component.add_global_includes('core/include')
component.add_global_includes('class/msd/include')
component.add_global_includes('class/hid/include')
